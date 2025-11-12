//
// File: BufferManager.hpp
// Description: Manages Vulkan buffer creation, allocation, and destruction
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"
#include "Types/BufferTypes.hpp"

namespace ThatEngine
{
    class BufferManager
    {
        public:
        BufferManager() = default;
        void Init(VkContext* content);
        
        Buffer AllocateBuffer(uint32_t dataSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
        void UploadData(Buffer& buffer, const void* data, uint32_t size, VkDeviceSize offset = 0);
        void CopyData(VkCommandBuffer& cmd, Buffer& source, Buffer& destination, VkDeviceSize dataSize, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset);
        void DestroyBuffer(Buffer& buffer);

        private:
        VkContext* m_Context;
    };
}