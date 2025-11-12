//
// File: FontManager.hpp
// Description: Manages font-related assets
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Renderer/ImageManager.hpp"
#include "Types/FontTypes.hpp"

namespace ThatEngine
{
    class FontManager
    {
        public:
        FontManager() = default;
        void Init(VkContext* context, ImageManager* imageManger);
        void Shutdown();

        inline const Shared<FontAtlasData>& GetFontAtlasData(FontAssetType type) const { return m_FontAtlases.at(type); }

        private:
        void LoadFont(FontAssetType fontType, TextureType textureType, const std::string& path);
        void CreateFontAtlas(const uint8_t* data, FontAssetType fontType, TextureType textureType);

        private:
        std::unordered_map<FontAssetType, Shared<FontAtlasData>> m_FontAtlases;

        VkContext* m_Context;
        ImageManager* m_ImageManager;
    };
}