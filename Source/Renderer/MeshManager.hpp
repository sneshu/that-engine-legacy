//
// File: MeshManager.hpp
// Description: Manages mesh assets, loads vertex / index data and uploads it to GPU
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"
#include "Renderer/BufferManager.hpp"
#include "Types/MeshTypes.hpp"

#include <unordered_map>

namespace ThatEngine
{
    class MeshManager
    {
        public:
        MeshManager() = default;
        void Init(VkContext* context, BufferManager* bufferManager);
        void Shutdown();

        void LoadMeshAsset(MeshAssetType type, const MeshAsset& asset);
        const MeshGPUData& GetMeshAssetGPUData(MeshAssetType type) const;
        const MeshAsset& GetMeshAsset(MeshAssetType type) const;

        private:
        MeshGPUData UploadMeshToGPU(const MeshAsset& asset);
        void LoadQuad();
        void LoadCube();

        private:
        VkContext* m_Context;
        BufferManager* m_BufferManager;
        std::unordered_map<MeshAssetType, MeshEntry> m_LoadedMeshAssets;
    };
}