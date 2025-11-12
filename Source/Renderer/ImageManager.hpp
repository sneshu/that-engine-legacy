//
// File: ImageManager.hpp
// Description: Manages Vulkan image creation, allocation, and destruction,
//              uploads images to GPU
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"
#include "Types/ImageTypes.hpp"

namespace ThatEngine
{
    class ImageManager
    {
        public:
        ImageManager() = default;
        void Init(VkContext* context);
        void LoadTextures();
        void Shutdown();

        Shared<Image> AllocateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage, VkImage imageHandle = VK_NULL_HANDLE);
        Shared<Image> CreateImage(const uint8_t* data, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage);
        Shared<Image> CreateImageFromFile(const std::string& path, VkFormat format, VkImageUsageFlags usage);
        void CreateImageView(const Shared<Image>& image, VkImageAspectFlags aspectMask);
        void DestroyImage(const Shared<Image>& image);

        void CreateTexture(TextureType type, const Shared<Image>& image);
        inline const Shared<Image>& GetTexture(TextureType type) const { return m_Textures.at(type); }

        private:
        void UploadImageToGPU(const Shared<Image>& image);
        void LoadTexture(TextureType type, const std::string& path, VkFormat format, VkImageUsageFlags usage);

        private:
        VkContext* m_Context;
        std::unordered_map<TextureType, Shared<Image>> m_Textures;
    };
}