//
// File: PipelineTypes.hpp
// Description: Defines pipeline-related structs used by managers
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Types/ShaderTypes.hpp"

namespace ThatEngine
{
    enum class PipelineType : uint32_t
    {
        None = 0,
        DefaultLit,
        DefaultLitWireframe,
        WorldSpaceText,
        WorldSpaceTextWireframe,
        ScreenSpaceText,
        PostProcessing,
        Count
    };
    
    struct PipelineCreateInfo
    {
        const std::string& Name;
        PipelineType Type;
        const Shared<ShaderProgram>& Program;
        VkRenderPass RenderPass;
        uint32_t SubpassIndex = 0;
        VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkSampleCountFlagBits RasterSamples = VK_SAMPLE_COUNT_1_BIT;
        VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace FrontFace = VK_FRONT_FACE_CLOCKWISE;
        VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL;
        bool EnableBlending = true;
        bool EnableDepthTesting = true;
    };

    // Used for binding descriptors
    enum class BoundResourceType : uint32_t
    {
        Image,
        UniformBuffer,
        StorageBuffer,
        ColorInputAttachment,
        DepthInputAttachment
    };

    struct BoundResource
    {
        BoundResourceType Type;
        VkDescriptorType DescriptorType;
        uint32_t Binding;
        // Descriptor binding info depending on BoundResourceType
        VkDescriptorImageInfo ImageInfo;
        VkDescriptorBufferInfo BufferInfo;
    };

    using BoundResources = std::unordered_map<uint32_t, BoundResource>;

    // Stored inside PipelineManager
    struct PipelineResources
    {
        VkPipeline Pipeline;
        VkPipelineLayout Layout;
        VkDescriptorSetLayout DescriptorSetLayout;
        VkDescriptorSet DescriptorSet;  
        BoundResources BoundResources;
    };
}