//
// File: ResourceManager.hpp
// Description: Centralized manager that handles graphics-related resource managers to 
//              simplify Renderer class and reduce its responsibilites
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"
#include "Renderer/BufferManager.hpp"
#include "Renderer/ImageManager.hpp"
#include "Renderer/ShaderManager.hpp"
#include "Renderer/MeshManager.hpp"
#include "Renderer/FontManager.hpp"

namespace ThatEngine
{
    class ResourceManager
    {
        public:
        ResourceManager() = default;
        void Init(VkContext* context)
        {
            m_Context = context;
            m_BufferManager.Init(m_Context);
            m_ShaderManager.Init(m_Context);
            m_ImageManager.Init(m_Context);
        }
        
        // Managers and their late init functions that need staging buffers
        void LateInit()
        {
            m_ImageManager.LoadTextures();
            m_MeshManager.Init(m_Context, &m_BufferManager);
            m_FontManager.Init(m_Context, &m_ImageManager);
        }

        void Shutdown()
        {
            m_ImageManager.Shutdown();
            m_MeshManager.Shutdown();
            m_ShaderManager.Shutdown();
            m_FontManager.Shutdown();
        }

        inline BufferManager& GetBufferManager() { return m_BufferManager; }
        inline ImageManager& GetImageManager() { return m_ImageManager; }
        inline MeshManager& GetMeshManager() { return m_MeshManager; }
        inline ShaderManager& GetShaderManager() { return m_ShaderManager; }
        inline FontManager& GetFontManager() { return m_FontManager; }

        private:
        VkContext* m_Context;
        BufferManager m_BufferManager;
        ImageManager m_ImageManager;
        MeshManager m_MeshManager;
        ShaderManager m_ShaderManager;
        FontManager m_FontManager;
    };
}