//
// File: MeshTypes.hpp
// Description: Defines mesh-related structs used by managers
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Types/BufferTypes.hpp"
#include "Types/ShaderTypes.hpp"

namespace ThatEngine
{
    enum class MeshAssetType : uint32_t
    {
        None = 0,
        Quad,
        Cube,
    };

    // Raw mesh data loaded from disk or generated
    struct MeshAsset
    {
        std::string Name;
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };

    // Contains GPU buffers with vertex and index data
    struct MeshGPUData
    {
        Buffer VertexBuffer;
        uint32_t VertexCount;
        Buffer IndexBuffer;
        uint32_t IndexCount;
    };

    struct MeshEntry
    {
        MeshAsset Asset;
        MeshGPUData GPUData; 
    };
}