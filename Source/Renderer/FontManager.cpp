//
// File: FontManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/FontManager.hpp"
#include "Types/ImageTypes.hpp"

namespace ThatEngine
{
    void FontManager::Init(VkContext* context, ImageManager* imageManager)
    {
        m_Context = context;
        m_ImageManager = imageManager;

        LoadFont(FontAssetType::Default, TextureType::DefaultFont, "Assets/Fonts/Mx437_Verite_9x14.ttf");
    }

    void FontManager::LoadFont(FontAssetType fontType, TextureType textureType, const std::string& path)
    {
        // Read binary data
        uint32_t fileSize;
        const uint8_t* data = FileReader::ReadBinaryFile<uint8_t>(path, fileSize);

        // Create font asset
        CreateFontAtlas(data, fontType, textureType);
        
        THAT_CORE_INFO("Font Manager: Loading Asset \"{}\"", path);
        delete[] data;
    }

    void FontManager::CreateFontAtlas(const uint8_t* data, FontAssetType fontType, TextureType textureType)
    {
        stbtt_fontinfo font;
        if (!stbtt_InitFont(&font, data, 0))
        {
            THAT_CORE_ERROR("Font Manager: Failed to initialize font!", 0);
            return;
        }

        constexpr uint32_t BITMAP_SIZE = 1024;
        constexpr uint32_t GLYPH_COUNT = 96;
        constexpr uint32_t GLYPH_OFFSET = 32;
        constexpr float GLYPH_SIZE = 126.0f;
 
        uint8_t* bitmapData = new uint8_t[BITMAP_SIZE * BITMAP_SIZE] {};
        auto glyphsRawData = CreateUnique<stbtt_bakedchar[]>(GLYPH_COUNT); 
        if (stbtt_BakeFontBitmap(data, 0, GLYPH_SIZE, bitmapData, BITMAP_SIZE, BITMAP_SIZE, GLYPH_OFFSET, GLYPH_COUNT, glyphsRawData.get()) <= 0)
        {
            THAT_CORE_ERROR("Font Manager: Failed to bake font bitmap!", 0);
            delete[] bitmapData;
            return;
        }

        auto glyphs = CreateUnique<GlyphData[]>(GLYPH_COUNT);
        for (uint32_t i = 0; i < GLYPH_COUNT; i++)
        {
            glyphs[i] = GlyphData
            {
                .Rect = glm::vec4(
                    glyphsRawData[i].x0, 
                    glyphsRawData[i].y0, 
                    (glyphsRawData[i].x1 - glyphsRawData[i].x0), 
                    (glyphsRawData[i].y1 - glyphsRawData[i].y0)
                ),
                .Offset = glm::vec2(glyphsRawData[i].xoff, glyphsRawData[i].yoff),
                .AdvanceX = glyphsRawData[i].xadvance
            };
        }

        Shared<Image> image = m_ImageManager->CreateImage(bitmapData, BITMAP_SIZE, BITMAP_SIZE, 1, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        m_ImageManager->CreateTexture(textureType, image);

        delete[] bitmapData;

        Shared<FontAtlasData> fontAtlasData = CreateShared<FontAtlasData>(); 
        fontAtlasData->Texture = textureType;
        fontAtlasData->FontSize = GLYPH_SIZE;
        fontAtlasData->Glyphs = std::move(glyphs);
        fontAtlasData->GlyphCount = GLYPH_COUNT;
        fontAtlasData->GlyphOffset = GLYPH_OFFSET;

        m_FontAtlases[fontType] = fontAtlasData;
    }

    void FontManager::Shutdown()
    {

    }
}