//
// File: FontTypes.hpp
// Description: Defines font-related structs used by managers
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Memory.hpp"
#include "Types/ImageTypes.hpp"

#include <stb_truetype.h>

namespace ThatEngine
{
    enum class FontAssetType : uint32_t
    {
        None = 0,
        Default
    };

    struct GlyphData
    {
        glm::vec4 Rect;
        glm::vec2 Offset;
        float AdvanceX;
    };

    // Used to store font asset data inside mesh manager
    struct FontAtlasData
    {
        TextureType Texture;
        float FontSize;
        Unique<GlyphData[]> Glyphs;
        uint32_t GlyphCount;
        uint32_t GlyphOffset;

        const GlyphData& GetGlyphData(uint32_t glyph)
        {
            return Glyphs[glyph - GlyphOffset];
        }
    };
}