//
// File: BufferTypes.hpp
// Description: Defines buffer struct type
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"

namespace ThatEngine
{
    struct Buffer
    {
        VkBuffer Buffer;
        VkDeviceMemory Memory;
        uint32_t Size;
        void* Data;
    };
}