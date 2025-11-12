//
// File: MeshManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/MeshManager.hpp"
#include "Renderer/VulkanUtils.hpp"

namespace ThatEngine
{
    void MeshManager::Init(VkContext* context, BufferManager* bufferManager)
    {
        m_Context = context;
        m_BufferManager = bufferManager;

        LoadQuad();
        LoadCube();
    }

    void MeshManager::Shutdown()
    {
        for (auto& [_, entry] : m_LoadedMeshAssets)
        {
            m_BufferManager->DestroyBuffer(entry.GPUData.VertexBuffer);
            m_BufferManager->DestroyBuffer(entry.GPUData.IndexBuffer);
        }

        m_LoadedMeshAssets.clear();
    }

    void MeshManager::LoadMeshAsset(MeshAssetType type, const MeshAsset& asset)
    {
        MeshGPUData data = UploadMeshToGPU(asset);
        MeshEntry entry = { asset, data };
        m_LoadedMeshAssets[type] = entry;

        THAT_CORE_INFO("Mesh Manager: Loading Asset \"{}\"", asset.Name);
    }

    const MeshGPUData& MeshManager::GetMeshAssetGPUData(MeshAssetType type) const
    {
        const auto& iterator = m_LoadedMeshAssets.find(type);
        
        THAT_CORE_ASSERT(iterator != m_LoadedMeshAssets.end(), "Mesh asset's GPU data not found!", 0);
        
        return iterator->second.GPUData;
    }

    const MeshAsset& MeshManager::GetMeshAsset(MeshAssetType type) const
    {
        const auto& iterator = m_LoadedMeshAssets.find(type);
        
        THAT_CORE_ASSERT(iterator != m_LoadedMeshAssets.end(), "Mesh asset not found!", 0);
        
        return iterator->second.Asset;
    }

    MeshGPUData MeshManager::UploadMeshToGPU(const MeshAsset& meshAsset)
    {
        MeshGPUData handle = {};

        uint32_t vertexBufferSize = sizeof(Vertex) * meshAsset.Vertices.size();
        uint32_t indexBufferSize = sizeof(uint32_t) * meshAsset.Indices.size();
        handle.VertexCount = static_cast<uint32_t>(meshAsset.Vertices.size());
        handle.IndexCount = static_cast<uint32_t>(meshAsset.Indices.size());
        
        // GPU-sided buffers
        handle.VertexBuffer = m_BufferManager->AllocateBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        handle.IndexBuffer = m_BufferManager->AllocateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        // Host-sided buffers
        Buffer stagingVertexBuffer = m_BufferManager->AllocateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        Buffer stagingIndexBuffer = m_BufferManager->AllocateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        // Upload data to staging buffers
        m_BufferManager->UploadData(stagingVertexBuffer, meshAsset.Vertices.data(), vertexBufferSize);
        m_BufferManager->UploadData(stagingIndexBuffer, meshAsset.Indices.data(), indexBufferSize);

        // Copy data to GPU local buffers
        {
            VkCommandBuffer cmd = VulkanUtils::BeginSingleTimeCommands(m_Context->Device, m_Context->CommandPool);

            VkBufferCopy vertexCopy = { 0, 0, vertexBufferSize };
            vkCmdCopyBuffer(cmd, stagingVertexBuffer.Buffer, handle.VertexBuffer.Buffer, 1, &vertexCopy);

            VkBufferCopy indexCopy = { 0, 0, indexBufferSize };
            vkCmdCopyBuffer(cmd, stagingIndexBuffer.Buffer, handle.IndexBuffer.Buffer, 1, &indexCopy);

            VulkanUtils::EndSingleTimeCommands(m_Context->Device, m_Context->GraphicsQueue, m_Context->CommandPool, cmd);
        }

        // Clear temporary buffers
        m_BufferManager->DestroyBuffer(stagingVertexBuffer);
        m_BufferManager->DestroyBuffer(stagingIndexBuffer);

        return handle;
    }

    void MeshManager::LoadQuad()
    {
        MeshAsset meshAsset = 
        {
            .Name = "Quad",
            .Vertices =  
            {{
                // Faces camera (-Z)
                {{ -0.5f,  0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 0.f, 0.f }},
                {{  0.5f,  0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 1.f, 0.f }},
                {{  0.5f, -0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 0.f, 1.f }},
                {{ -0.5f,  0.5f,  0.0f }, {  0.f,  0.f, -1.f }, { 0.f, 0.f }},
            }},
            .Indices = 
            {
                0, 1, 2, 3, 4, 5,
            }
        };

        LoadMeshAsset(MeshAssetType::Quad, meshAsset);
    }

    void MeshManager::LoadCube()
    {
        MeshAsset meshAsset = 
        {
            .Name = "Cube",
            .Vertices =  
            {{
                // Front face (+Z)
                {{  0.5f,  0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 0.f, 0.f }},
                {{ -0.5f,  0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 1.f, 0.f }},
                {{ -0.5f, -0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 0.f, 1.f }},
                {{  0.5f,  0.5f,  0.5f }, {  0.f,  0.f,  1.f }, { 0.f, 0.f }},

                // Back face (-Z)
                {{ -0.5f,  0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 0.f, 0.f }},
                {{  0.5f,  0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 1.f, 0.f }},
                {{  0.5f, -0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 0.f, 1.f }},
                {{ -0.5f,  0.5f, -0.5f }, {  0.f,  0.f, -1.f }, { 0.f, 0.f }},

                // Right face (+X)
                {{  0.5f,  0.5f, -0.5f }, {  1.f,  0.f,  0.f }, { 0.f, 0.f }},
                {{  0.5f,  0.5f,  0.5f }, {  1.f,  0.f,  0.f }, { 1.f, 0.f }},
                {{  0.5f, -0.5f,  0.5f }, {  1.f,  0.f,  0.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f,  0.5f }, {  1.f,  0.f,  0.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f, -0.5f }, {  1.f,  0.f,  0.f }, { 0.f, 1.f }},
                {{  0.5f,  0.5f, -0.5f }, {  1.f,  0.f,  0.f }, { 0.f, 0.f }},

                // Left face (-X)
                {{ -0.5f,  0.5f,  0.5f }, { -1.f,  0.f,  0.f }, { 0.f, 0.f }},
                {{ -0.5f,  0.5f, -0.5f }, { -1.f,  0.f,  0.f }, { 1.f, 0.f }},
                {{ -0.5f, -0.5f, -0.5f }, { -1.f,  0.f,  0.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f, -0.5f }, { -1.f,  0.f,  0.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f,  0.5f }, { -1.f,  0.f,  0.f }, { 0.f, 1.f }},
                {{ -0.5f,  0.5f,  0.5f }, { -1.f,  0.f,  0.f }, { 0.f, 0.f }},

                // Top face (+Y)
                {{ -0.5f,  0.5f,  0.5f }, {  0.f,  1.f,  0.f }, { 0.f, 0.f }},
                {{  0.5f,  0.5f,  0.5f }, {  0.f,  1.f,  0.f }, { 1.f, 0.f }},
                {{  0.5f,  0.5f, -0.5f }, {  0.f,  1.f,  0.f }, { 1.f, 1.f }},
                {{  0.5f,  0.5f, -0.5f }, {  0.f,  1.f,  0.f }, { 1.f, 1.f }},
                {{ -0.5f,  0.5f, -0.5f }, {  0.f,  1.f,  0.f }, { 0.f, 1.f }},
                {{ -0.5f,  0.5f,  0.5f }, {  0.f,  1.f,  0.f }, { 0.f, 0.f }},

                // Bottom face (-Y)
                {{ -0.5f, -0.5f, -0.5f }, {  0.f, -1.f,  0.f }, { 0.f, 0.f }},
                {{  0.5f, -0.5f, -0.5f }, {  0.f, -1.f,  0.f }, { 1.f, 0.f }},
                {{  0.5f, -0.5f,  0.5f }, {  0.f, -1.f,  0.f }, { 1.f, 1.f }},
                {{  0.5f, -0.5f,  0.5f }, {  0.f, -1.f,  0.f }, { 1.f, 1.f }},
                {{ -0.5f, -0.5f,  0.5f }, {  0.f, -1.f,  0.f }, { 0.f, 1.f }},
                {{ -0.5f, -0.5f, -0.5f }, {  0.f, -1.f,  0.f }, { 0.f, 0.f }},
            }},
            .Indices = 
            {
                0, 1, 2, 3, 4, 5,
                6, 7, 8, 9, 10, 11,
                12, 13, 14, 15, 16, 17,
                18, 19, 20, 21, 22, 23,
                24, 25, 26, 27, 28, 29, 
                30, 31, 32, 33, 34, 35,
            },
        };

        LoadMeshAsset(MeshAssetType::Cube, meshAsset);
    }
}