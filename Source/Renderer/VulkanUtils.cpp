//
// File: VulkanUtils.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/Vulkan.hpp"

namespace ThatEngine
{   
    namespace VulkanUtils
    {
        VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags)
        {
            VkFenceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.flags = flags;

            VkFence fence;
            VK_CHECK(vkCreateFence(device, &info, 0, &fence));

            return fence;
        }

        VkSubmitInfo CreateSubmitInfo(VkCommandBuffer* commandBuffer, uint32_t commandCount)
        {
            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.commandBufferCount = commandCount;
            info.pCommandBuffers = commandBuffer;
            return info;
        }

        VkCommandBufferBeginInfo CreateCommandBufferBeginInfo()
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            return info;
        }

        VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo(VkCommandPool commandPool)
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.commandBufferCount = 1;
            info.commandPool = commandPool;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            return info;
        }

        VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
        {
            VkCommandBuffer cmd;
            VkCommandBufferAllocateInfo allocInfo = CreateCommandBufferAllocateInfo(commandPool);
            VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &cmd));

            VkCommandBufferBeginInfo beginInfo = CreateCommandBufferBeginInfo();
            VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

            return cmd;
        }

        void EndSingleTimeCommands(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer)
        {
            VK_CHECK(vkEndCommandBuffer(commandBuffer));

            VkFence uploadFence = CreateFence(device, 0);
            VkSubmitInfo submitInfo = CreateSubmitInfo(&commandBuffer, 1);
            VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, uploadFence));

            vkWaitForFences(device, 1, &uploadFence, VK_TRUE, UINT64_MAX);
            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
            vkDestroyFence(device, uploadFence, 0);
        }

        uint32_t GetMemoryTypeIndex(VkPhysicalDevice gpu, VkMemoryRequirements memoryReqs, VkMemoryPropertyFlags memoryPropFlags)
        {
            uint32_t memoryTypeId = INVALID_UINT32_ID;
            
            VkPhysicalDeviceMemoryProperties gpuMemoryProps;
            vkGetPhysicalDeviceMemoryProperties(gpu, &gpuMemoryProps);
            
            for (uint32_t i = 0; i < gpuMemoryProps.memoryTypeCount; i++)
            {
                if (memoryReqs.memoryTypeBits & (1 << i) && (gpuMemoryProps.memoryTypes[i].propertyFlags & memoryPropFlags) == memoryPropFlags)
                {
                    memoryTypeId = i;
                    break;
                }
            }

            THAT_CORE_ASSERT(memoryTypeId != INVALID_UINT32_ID, "Failed to find index for proper memory properties: {}", memoryPropFlags);

            return memoryTypeId;
        }

        uint32_t GetFormatSize(VkFormat format)
        {
            switch (format) 
            {
                case VK_FORMAT_R8_UNORM:
                    return 1;
                case VK_FORMAT_R32_SFLOAT: 
                case VK_FORMAT_B8G8R8A8_UNORM:
                case VK_FORMAT_R8G8B8A8_UNORM:
                    return 4;
                case VK_FORMAT_R32G32_SFLOAT: 
                    return 8;
                case VK_FORMAT_R32G32B32_SFLOAT: 
                    return 12;
                case VK_FORMAT_R32G32B32A32_SFLOAT: 
                    return 16;
                default: 
                {
                    THAT_CORE_ERROR("Unsupported VkFormat: {}", static_cast<uint32_t>(format));
                    return 0;
                }
            }
        }

        void MergeDescriptorBindings(std::vector<VkDescriptorSetLayoutBinding>& merged, const std::vector<VkDescriptorSetLayoutBinding>& incoming, VkShaderStageFlagBits stage)
        {
            std::unordered_map<uint32_t, uint32_t> bindingIndexMap;

            for (uint32_t i = 0; i < merged.size(); ++i)
            {
                bindingIndexMap[merged[i].binding] = i;
            }

            for (const VkDescriptorSetLayoutBinding& incomingBinding : incoming)
            {
                auto iterator = bindingIndexMap.find(incomingBinding.binding);

                if (iterator != bindingIndexMap.end())
                {
                    merged[iterator->second].stageFlags |= stage;
                }

                else
                {
                    VkDescriptorSetLayoutBinding binding = incomingBinding;
                    binding.stageFlags = stage;
                    merged.emplace_back(binding);
                    bindingIndexMap[binding.binding] = merged.size() - 1;
                }
            }
        }

        // Second vector is merged into first
        void MergePushConstantRanges(std::vector<VkPushConstantRange>& first, const std::vector<VkPushConstantRange>& second, VkShaderStageFlagBits stage)
        {
            for (const VkPushConstantRange& secondRange : second)
            {
                bool merged = false;

                for (VkPushConstantRange& firstRange : first)
                {
                    if (firstRange.offset == secondRange.offset && firstRange.size == secondRange.size)
                    {
                        firstRange.stageFlags |= stage;
                        merged = true;
                        break;
                    }
                }

                if (!merged)
                {
                    VkPushConstantRange range = secondRange;
                    range.stageFlags = stage;
                    first.emplace_back(range);
                }
            }
        }

        VkSurfaceFormatKHR FindSurfaceFormat(VkPhysicalDevice gpu, VkSurfaceKHR surface)
        {
            uint32_t formatCount = 0;
            std::array<VkSurfaceFormatKHR, 16> surfaceFormats;
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, 0);
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfaceFormats.data());
            
            for (uint32_t i = 0; i < formatCount; i++)
            {
                VkSurfaceFormatKHR surfaceFormat = surfaceFormats[i];
                
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
                {
                    return surfaceFormat;
                }
            }
            
            THAT_CORE_INFO("Vulkan: Error during initialization - no surface format found!");
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
        }

        VkFormat FindSupportedFormat(VkPhysicalDevice gpu, const std::vector<VkFormat>& candidates)
        {
            for (VkFormat format : candidates)
            {
                VkFormatProperties properties;
                vkGetPhysicalDeviceFormatProperties(gpu, format, &properties);

                if (properties.optimalTilingFeatures != 0 || properties.linearTilingFeatures != 0)
                {
                    return format;
                }
            }

            return { VK_FORMAT_B8G8R8A8_UNORM };
        }   
    }
}