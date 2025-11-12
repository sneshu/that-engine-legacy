//
// File: ShaderTypes.hpp
// Description: Defines shader-related structs used by managers
//              but also structs that are inside shader programs
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"

#include <glm/glm.hpp>

namespace ThatEngine
{
    enum class ShaderProgramType : uint32_t
    {
        None = 0,
        DefaultLit,
        WorldSpaceText,
        ScreenSpaceText,
        PostProcessing,
        Count
    };

    struct ShaderModule
    {
        std::string EntryPoint;
        VkShaderStageFlagBits Stage;
        VkShaderModule Handle;

        std::vector<VkDescriptorSetLayoutBinding> DescriptorBindings;
        std::vector<VkPushConstantRange> PushConstantRanges;
        std::vector<VkVertexInputAttributeDescription> VertexAttributeDescriptions;
        std::vector<VkVertexInputBindingDescription> VertexBindingDescriptions;
    };

    struct ShaderProgram
    {
        Shared<ShaderModule> Vertex;
        Shared<ShaderModule> Fragment;

        std::vector<VkDescriptorSetLayoutBinding> MergedDescriptorBindings;
        std::vector<VkPushConstantRange> MergedPushConstantRanges;
    };

    // Structs used inside shaders
    // Remember to add padding to align them to 16 bytes
    struct GlobalData
    {
        glm::vec4 ScreenSize;                   // 16 bytes - Screen width, screen height, camera near, camera far
        glm::mat4 PerspectiveViewProjection;    // 64 bytes
        glm::mat4 OrthographicViewProjection;   // 64 bytes
        glm::vec4 SkyColor;                     // 16 bytes
        glm::vec4 LightColor;                   // 16 bytes
        glm::vec3 LightDirection;               // 12 bytes -> aligned to 16 bytes
        uint32_t _padding0;
        glm::vec2 FogParams;                    // 8 bytes -> aligned to 16 bytes
        uint32_t _padding1[2];
        uint32_t RenderMode;                    // 2x4 bytes -> aligned to 16 bytes
        float Time;                             
        uint32_t _padding2[2];
    };
    
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
    };

    struct MeshInstance
    {
        glm::mat4 Model;
    };

    struct GlyphInstance
    {
        glm::mat4 Model;
        glm::vec4 Rect;
        glm::vec4 Color;
    };
}