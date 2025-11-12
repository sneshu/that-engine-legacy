//
// File: ImageTypes.hpp
// Description: Defines image struct type
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"

namespace ThatEngine
{
    struct Image
    {
        VkImage Image;
        VkImageView View;
        VkDeviceMemory Memory;
        VkFormat Format;
        uint32_t Width;
        uint32_t Height;
        uint32_t MipLevels;
    };

    enum class TextureType : uint32_t
    {
        None = 0,
        DefaultFont,
        BlockDirt,
        BlockSand,
        BlockWhiteTile,
    };
}