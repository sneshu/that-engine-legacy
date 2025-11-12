//
// File: BufferManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/Vulkan.hpp"
#include "Renderer/VulkanUtils.hpp"
#include "Renderer/BufferManager.hpp"

namespace ThatEngine
{
    void BufferManager::Init(VkContext* context)
    {
        m_Context = context;
    }

    Buffer BufferManager::AllocateBuffer(uint32_t dataSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
    {
        Buffer buffer = {};
        buffer.Size = dataSize;

        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.size = dataSize;
        info.usage = usage;
        VK_CHECK(vkCreateBuffer(m_Context->Device, &info, 0, &buffer.Buffer));
        
        VkMemoryRequirements memoryReqs;
        vkGetBufferMemoryRequirements(m_Context->Device, buffer.Buffer, &memoryReqs);

        uint32_t memoryTypeId = VulkanUtils::GetMemoryTypeIndex(m_Context->Gpu, memoryReqs, memoryProperties);
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.memoryTypeIndex = memoryTypeId;
        allocInfo.allocationSize = buffer.Size;
        VK_CHECK(vkAllocateMemory(m_Context->Device, &allocInfo, 0, &buffer.Memory));
        VK_CHECK(vkBindBufferMemory(m_Context->Device, buffer.Buffer, buffer.Memory, 0));

        return buffer;
    }

    void BufferManager::UploadData(Buffer& buffer, const void* data, uint32_t size, VkDeviceSize offset)
    {
        buffer.Data = nullptr;
        VK_CHECK(vkMapMemory(m_Context->Device, buffer.Memory, offset, size, 0, &buffer.Data));
        memcpy(buffer.Data, data, size);
        vkUnmapMemory(m_Context->Device, buffer.Memory);
    }

    void BufferManager::CopyData(VkCommandBuffer& cmd, Buffer& source, Buffer& destination, VkDeviceSize dataSize, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset)
    {
        VkBufferCopy copyRegion = { sourceOffset, destinationOffset, dataSize };
        vkCmdCopyBuffer(cmd, source.Buffer, destination.Buffer, 1, &copyRegion);
    }

    void BufferManager::DestroyBuffer(Buffer& buffer)
    {
        if (buffer.Buffer)
        {
            vkDestroyBuffer(m_Context->Device, buffer.Buffer, nullptr);
            buffer.Buffer = VK_NULL_HANDLE;
        }

        if (buffer.Memory)
        {
            vkFreeMemory(m_Context->Device, buffer.Memory, nullptr);
            buffer.Memory = VK_NULL_HANDLE;
        }
    }
}