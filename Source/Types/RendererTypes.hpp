//
// File: RendererTypes.hpp
// Description: Defines render-related structs used by managers
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Types/MeshTypes.hpp"
#include "Types/ShaderTypes.hpp"

namespace ThatEngine
{
    enum class RenderMode : uint32_t
    {
        Color,
        Depth,
        Normals,
        Triangles,
        Wireframe,
        Count
    };

    constexpr const char* RenderModeNames[static_cast<uint32_t>(RenderMode::Count)] = { "COLOR", "DEPTH", "NORMALS", "TRIANGLES", "WIREFRAME" };
    inline const char* ToString(RenderMode mode) { return RenderModeNames[static_cast<uint32_t>(mode)]; }

    template<typename T>
    struct InstanceBatch
    {
        std::vector<T> Instances;
        VkDeviceSize FirstInstance;
    };

    struct RenderableDatapack
    {
        glm::vec4 ClearColor;
        
        std::unordered_map<MeshAssetType, InstanceBatch<MeshInstance>> MeshInstanceBatches;
        VkDeviceSize MeshInstanceBatchesOffset;

        std::unordered_map<FontAssetType, InstanceBatch<GlyphInstance>> WorldSpaceGlyphInstanceBatches;
        VkDeviceSize WorldSpaceGlyphInstanceBatchesOffset;

        std::unordered_map<FontAssetType, InstanceBatch<GlyphInstance>> ScreenSpaceGlyphInstanceBatches;
        VkDeviceSize ScreenSpaceGlyphInstanceBatchesOffset;

        // Clear so it doesn't rebuild hashmaps every frame
        void Clear()
        {
            for (auto& [_, batch] : MeshInstanceBatches) 
            {
                batch.Instances.clear();
            }

            for (auto& [_, batch] : WorldSpaceGlyphInstanceBatches)
            {
                batch.Instances.clear();
            }

            for (auto& [_, batch] : ScreenSpaceGlyphInstanceBatches) 
            {
                batch.Instances.clear();
            }
        }
    };
}