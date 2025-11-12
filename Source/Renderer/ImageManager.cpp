//
// File: ImageManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/Vulkan.hpp"
#include "Renderer/VulkanUtils.hpp"
#include "Renderer/ImageManager.hpp"
#include "Types/DDSFormatTypes.hpp"
#include "Types/ImageTypes.hpp"

namespace ThatEngine
{
    void ImageManager::Init(VkContext* context)
    {
        m_Context = context;
    }

    void ImageManager::LoadTextures()
    {
        LoadTexture(TextureType::BlockDirt, "Assets/Textures/Blocks/T_Block_Dirt.dds", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        LoadTexture(TextureType::BlockSand, "Assets/Textures/Blocks/T_Block_Sand.dds", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        LoadTexture(TextureType::BlockWhiteTile, "Assets/Textures/Blocks/T_Block_WhiteTile.dds", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    }

    void ImageManager::Shutdown()
    {
        for (auto& [_, image] : m_Textures)
        {
            DestroyImage(image);
        }
    }

    Shared<Image> ImageManager::AllocateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage, VkImage imageHandle)
    {
        Shared<Image> image = CreateShared<Image>();
        image->Format = format;
        image->Width = width;
        image->Height = height;
        image->MipLevels = mipLevels;

        // Case for swapchain images
        if (imageHandle != VK_NULL_HANDLE)
        {
            image->Image = imageHandle;
            return image;
        }

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.extent = { width, height, 1 };
        info.mipLevels = mipLevels;
        info.arrayLayers = 1;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.format = format;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.usage = usage;
        
        VK_CHECK(vkCreateImage(m_Context->Device, &info, 0, &image->Image));

        VkMemoryRequirements memoryReqs;
        vkGetImageMemoryRequirements(m_Context->Device, image->Image, &memoryReqs);

        uint32_t memoryTypeId = VulkanUtils::GetMemoryTypeIndex(m_Context->Gpu, memoryReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.memoryTypeIndex = memoryTypeId;
        allocInfo.allocationSize = memoryReqs.size;
        VK_CHECK(vkAllocateMemory(m_Context->Device, &allocInfo, 0, &image->Memory));
        VK_CHECK(vkBindImageMemory(m_Context->Device, image->Image, image->Memory, 0));

        return image;
    }

    // Note: used data is not deleted! Manual cleanup needed.
    Shared<Image> ImageManager::CreateImage(const uint8_t* data, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage)
    {
        uint32_t imageSize = width * height * VulkanUtils::GetFormatSize(format);
        vkMapMemory(m_Context->Device, m_Context->ImageStagingBuffer.Memory, 0, m_Context->ImageStagingBuffer.Size, 0, &m_Context->ImageStagingBuffer.Data);
        memcpy(m_Context->ImageStagingBuffer.Data, data, imageSize);
        
        Shared<Image> image = AllocateImage(width, height, mipLevels, format, usage);
        UploadImageToGPU(image);

        return image;
    }

    Shared<Image> ImageManager::CreateImageFromFile(const std::string& path, VkFormat format, VkImageUsageFlags usage)
    {
        uint32_t fileSize;
        const uint8_t* rawData = FileReader::ReadBinaryFile<uint8_t>(path, fileSize);
        const DDSHeader* header = reinterpret_cast<const DDSHeader*>(rawData + 4); // Skip 4 bytes of magic numbers
        const uint8_t* imageData = rawData + 4 + sizeof(DDSHeader);

        Shared<Image> image = CreateImage(imageData, header->Width, header->Height, header->MipMapCount, format, usage);
        delete[] rawData;

        return image;
    }

    void ImageManager::CreateImageView(const Shared<Image>& image, VkImageAspectFlags aspectMask)
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image->Image;
        info.format = image->Format;
        info.subresourceRange.aspectMask = aspectMask;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.levelCount = image->MipLevels;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;

        VK_CHECK(vkCreateImageView(m_Context->Device, &info, 0, &image->View));
    }

    void ImageManager::DestroyImage(const Shared<Image>& image)
    {
        if (image->View)
        {
            vkDestroyImageView(m_Context->Device, image->View, nullptr);
            image->View = VK_NULL_HANDLE;
        }

        if (image->Image)
        {
            vkDestroyImage(m_Context->Device, image->Image, nullptr);
            image->Image = VK_NULL_HANDLE;
        }

        if (image->Memory)
        {
            vkFreeMemory(m_Context->Device, image->Memory, nullptr);
            image->Memory = VK_NULL_HANDLE;
        }
    }

    void ImageManager::UploadImageToGPU(const Shared<Image>& image)
    {
        // Start command buffer
        VkCommandBuffer cmd = VulkanUtils::BeginSingleTimeCommands(m_Context->Device, m_Context->CommandPool);
        
        // Transition layout to transfer optimal
        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.layerCount = 1;
        range.levelCount = image->MipLevels;

        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.image = image->Image;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.subresourceRange = range;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &imageMemoryBarrier);

        VkDeviceSize offset = 0;
        uint32_t mipWidth = image->Width;
        uint32_t mipHeight = image->Height;
        for (uint32_t i = 0; i < image->MipLevels; i++)
        {
            VkBufferImageCopy copyRegion = {};
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = i;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = { mipWidth, mipHeight, 1 };
            copyRegion.bufferOffset = offset;

            vkCmdCopyBufferToImage(cmd, m_Context->ImageStagingBuffer.Buffer, image->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            offset += mipWidth * mipHeight * VulkanUtils::GetFormatSize(image->Format);
            mipWidth = glm::max(1u, mipWidth / 2);
            mipHeight = glm::max(1u, mipHeight / 2);
        }

        vkUnmapMemory(m_Context->Device, m_Context->ImageStagingBuffer.Memory);
        
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &imageMemoryBarrier);

        CreateImageView(image, VK_IMAGE_ASPECT_COLOR_BIT);

        // End command buffer
        VulkanUtils::EndSingleTimeCommands(m_Context->Device, m_Context->GraphicsQueue, m_Context->CommandPool, cmd);        
    }

    void ImageManager::LoadTexture(TextureType type, const std::string& path, VkFormat format, VkImageUsageFlags usage)
    {
        Shared<Image> image = CreateImageFromFile(path, format, usage);
        CreateTexture(type, image);

        THAT_CORE_INFO("Image Manager: Loading Asset \"{}\"", path);
    }

    void ImageManager::CreateTexture(TextureType type, const Shared<Image>& image)
    {
        m_Textures[type] = image;
    }
}