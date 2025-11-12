//
// File: PipelineManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/PipelineManager.hpp"
#include "Renderer/Vulkan.hpp"
#include "Types/ShaderTypes.hpp"

namespace ThatEngine
{
    void PipelineManager::Init(VkContext* context, ShaderManager* shaderManager)
    {
        m_Context = context; 
        m_ShaderManager = shaderManager;

        // Default lit pipeline
        {
            CreatePipeline
            ({
                .Name = "Default Lit",
                .Type = PipelineType::DefaultLit,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::DefaultLit, "Assets/Shaders/DefaultLit.vert.spv", "Assets/Shaders/DefaultLit.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 0,
            });

            PrepareResourceBinding(PipelineType::DefaultLit, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::DefaultLit, 1, BoundResourceType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            PrepareResourceBinding(PipelineType::DefaultLit, 2, BoundResourceType::Image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

            // Wireframe variant
            CreatePipeline
            ({
                .Name = "Default Lit Wireframe",
                .Type = PipelineType::DefaultLitWireframe,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::DefaultLit, "Assets/Shaders/DefaultLit.vert.spv", "Assets/Shaders/DefaultLit.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 0,
                .PolygonMode = VK_POLYGON_MODE_LINE
            });

            PrepareResourceBinding(PipelineType::DefaultLitWireframe, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::DefaultLitWireframe, 1, BoundResourceType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            PrepareResourceBinding(PipelineType::DefaultLitWireframe, 2, BoundResourceType::Image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        // World-space text pipeline
        {
            CreatePipeline
            ({
                .Name = "World-space Text",
                .Type = PipelineType::WorldSpaceText,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::WorldSpaceText, "Assets/Shaders/WorldSpaceGlyph.vert.spv", "Assets/Shaders/WorldSpaceGlyph.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 0,
            });

            PrepareResourceBinding(PipelineType::WorldSpaceText, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::WorldSpaceText, 1, BoundResourceType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            PrepareResourceBinding(PipelineType::WorldSpaceText, 2, BoundResourceType::Image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

            // Wireframe variant
            CreatePipeline
            ({
                .Name = "World-space Text Wireframe",
                .Type = PipelineType::WorldSpaceTextWireframe,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::WorldSpaceText, "Assets/Shaders/WorldSpaceGlyph.vert.spv", "Assets/Shaders/WorldSpaceGlyph.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 0,
                .PolygonMode = VK_POLYGON_MODE_LINE,
            });

            PrepareResourceBinding(PipelineType::WorldSpaceTextWireframe, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::WorldSpaceTextWireframe, 1, BoundResourceType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            PrepareResourceBinding(PipelineType::WorldSpaceTextWireframe, 2, BoundResourceType::Image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }
        
        // Screen-space text pipeline
        {
            CreatePipeline
            ({
                .Name = "Screen-space Text",
                .Type = PipelineType::ScreenSpaceText,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::ScreenSpaceText, "Assets/Shaders/ScreenSpaceGlyph.vert.spv", "Assets/Shaders/ScreenSpaceGlyph.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 0,
            });

            PrepareResourceBinding(PipelineType::ScreenSpaceText, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::ScreenSpaceText, 1, BoundResourceType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            PrepareResourceBinding(PipelineType::ScreenSpaceText, 2, BoundResourceType::Image, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        }

        // Post-processing pipeline
        {
            CreatePipeline
            ({
                .Name = "Post-processing",
                .Type = PipelineType::PostProcessing,
                .Program = m_ShaderManager->CreateProgram(ShaderProgramType::PostProcessing, "Assets/Shaders/PostProcessing.vert.spv", "Assets/Shaders/PostProcessing.frag.spv"),
                .RenderPass = m_Context->RenderPass,
                .SubpassIndex = 1,
                .EnableDepthTesting = false,
            });

            PrepareResourceBinding(PipelineType::PostProcessing, 0, BoundResourceType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            PrepareResourceBinding(PipelineType::PostProcessing, 3, BoundResourceType::ColorInputAttachment, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
            PrepareResourceBinding(PipelineType::PostProcessing, 4, BoundResourceType::DepthInputAttachment, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
        }
    }

    void PipelineManager::Shutdown()
    {
        for (auto& [_, resources] : m_Pipelines)
        {
            vkDestroyDescriptorSetLayout(m_Context->Device, resources->DescriptorSetLayout, nullptr);
            vkDestroyPipelineLayout(m_Context->Device, resources->Layout, nullptr);
            vkDestroyPipeline(m_Context->Device, resources->Pipeline, nullptr);
        }
    }

    void PipelineManager::BindPipeline(const VkCommandBuffer& cmd, PipelineType type)
    {
        if (m_CurrentBoundPipelineType != type)
        {
            m_CurrentBoundPipelineType = type;
            m_Context->ActivePipeline = m_Pipelines[type];
        }

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->ActivePipeline->Layout, 0, 1, &m_Context->ActivePipeline->DescriptorSet, 0, 0);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->ActivePipeline->Pipeline);
    }

    void PipelineManager::PrepareResourceBinding(PipelineType type, uint32_t binding, BoundResourceType resourceType, VkDescriptorType descriptorType)
    {
        BoundResource resource = {};
        resource.Type = resourceType;
        resource.DescriptorType = descriptorType;

        m_Pipelines[type]->BoundResources[binding] = resource;
    }

    void PipelineManager::BindImageResource(PipelineType type, uint32_t binding, const Shared<Image>& image)
    {
        m_Pipelines[type]->BoundResources[binding].ImageInfo =
        {
            .sampler = m_Context->Sampler,
            .imageView = image->View,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
    }

    void PipelineManager::BindInputAttachmentResource(PipelineType type, uint32_t binding, const Shared<Image>& image, VkImageLayout imageLayout)
    {
        m_Pipelines[type]->BoundResources[binding].ImageInfo = 
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = image->View,
            .imageLayout = imageLayout,
        };
    }

    void PipelineManager::BindBufferResource(PipelineType type, uint32_t binding, Buffer buffer, VkDeviceSize offset, VkDeviceSize range)
    {
        m_Pipelines[type]->BoundResources[binding].BufferInfo =
        {
            .buffer = buffer.Buffer,
            .offset = offset,
            .range = range
        };
    }

    void PipelineManager::UpdateDescriptorSets()
    {
        for (const auto& [type, resources] : m_Pipelines)
        {
            if (!m_ActivePipelines.test(static_cast<uint32_t>(type))) continue;

            std::vector<VkWriteDescriptorSet> writes;
            writes.reserve(resources->BoundResources.size());

            for (const auto& [binding, resource] : resources->BoundResources)
            {
                VkWriteDescriptorSet write = {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = resources->DescriptorSet;
                write.dstBinding = binding;
                write.descriptorCount = 1;
                write.descriptorType = resource.DescriptorType;

                switch (resource.Type)
                {
                    case BoundResourceType::Image:
                    case BoundResourceType::ColorInputAttachment:
                    case BoundResourceType::DepthInputAttachment:
                    {
                        write.pImageInfo = &resource.ImageInfo;
                        break;
                    }

                    case BoundResourceType::UniformBuffer:
                    case BoundResourceType::StorageBuffer:
                    {
                        write.pBufferInfo = &resource.BufferInfo;
                        break;
                    }
                }

                writes.emplace_back(write);
            }

            vkUpdateDescriptorSets(m_Context->Device, writes.size(), writes.data(), 0, nullptr); 
        }
    }

    void PipelineManager::UpdatePipelineBoundBufferOffset(PipelineType type, uint32_t binding, VkDeviceSize offset)
    {
        m_Pipelines.at(type)->BoundResources[binding].BufferInfo.offset = offset;
    }

    bool PipelineManager::CreatePipeline(const PipelineCreateInfo& pipelineInfo)
    {
        m_Pipelines[pipelineInfo.Type] = CreateShared<PipelineResources>();
        PipelineResources* resources = m_Pipelines.at(pipelineInfo.Type).get();

        // Descriptor Set Layout
        VkDescriptorSetLayout descriptorSetLayout;
        {
            VkDescriptorSetLayoutCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            info.pBindings = pipelineInfo.Program->MergedDescriptorBindings.data();
            info.bindingCount = static_cast<uint32_t>(pipelineInfo.Program->MergedDescriptorBindings.size());

            VK_CHECK(vkCreateDescriptorSetLayout(m_Context->Device, &info, nullptr, &resources->DescriptorSetLayout));
        }

        // Descriptor Set
        {
            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_Context->DescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &resources->DescriptorSetLayout;

            VK_CHECK(vkAllocateDescriptorSets(m_Context->Device, &allocInfo, &resources->DescriptorSet));
        }

        // Pipeline layout
        VkPipelineLayout pipelineLayout;
        {
            VkPipelineLayoutCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.setLayoutCount = 1;
            info.pSetLayouts = &resources->DescriptorSetLayout;
            info.pPushConstantRanges = pipelineInfo.Program->MergedPushConstantRanges.data();
            info.pushConstantRangeCount = static_cast<uint32_t>(pipelineInfo.Program->MergedPushConstantRanges.size());

            VK_CHECK(vkCreatePipelineLayout(m_Context->Device, &info, nullptr, &resources->Layout));
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pVertexBindingDescriptions = pipelineInfo.Program->Vertex->VertexBindingDescriptions.data();
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(pipelineInfo.Program->Vertex->VertexBindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = pipelineInfo.Program->Vertex->VertexAttributeDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipelineInfo.Program->Vertex->VertexAttributeDescriptions.size());

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = pipelineInfo.Topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = pipelineInfo.PolygonMode;
        rasterizer.cullMode = pipelineInfo.CullMode;
        rasterizer.frontFace = pipelineInfo.FrontFace;
        rasterizer.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = pipelineInfo.RasterSamples;

        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthBoundsTestEnable = VK_FALSE;

        if(pipelineInfo.EnableDepthTesting)
        {
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.stencilTestEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        }

        else
        {
            depthStencil.depthTestEnable = VK_FALSE;
            depthStencil.depthWriteEnable = VK_FALSE;
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        }

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = pipelineInfo.EnableBlending ? VK_TRUE : VK_FALSE;

        if (pipelineInfo.EnableBlending)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &m_Context->Viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &m_Context->Scissor;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        VkPipelineShaderStageCreateInfo vertexShaderStage = {};
        vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStage.module = pipelineInfo.Program->Vertex->Handle;
        vertexShaderStage.pName = pipelineInfo.Program->Vertex->EntryPoint.c_str();

        VkPipelineShaderStageCreateInfo fragmentShaderStage = {};
        fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStage.module = pipelineInfo.Program->Fragment->Handle;
        fragmentShaderStage.pName = pipelineInfo.Program->Fragment->EntryPoint.c_str();

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStage, fragmentShaderStage };

        // Pipeline
        VkPipeline pipeline;
        {
            VkGraphicsPipelineCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            info.stageCount = 2;
            info.pStages = shaderStages;
            info.pVertexInputState = &vertexInputInfo;
            info.pInputAssemblyState = &inputAssembly;
            info.pRasterizationState = &rasterizer;
            info.pMultisampleState = &multisampling;
            info.pDepthStencilState = &depthStencil;
            info.pColorBlendState = &colorBlending;
            info.pViewportState = &viewportState;
            info.pDynamicState = &dynamicState;
            info.layout = resources->Layout;
            info.renderPass = pipelineInfo.RenderPass;
            info.subpass = pipelineInfo.SubpassIndex;

            VK_CHECK(vkCreateGraphicsPipelines(m_Context->Device, VK_NULL_HANDLE, 1, &info, nullptr, &resources->Pipeline));
        }

        THAT_CORE_INFO("Pipeline Manager: Loading Asset \"{}\"", pipelineInfo.Name);

        return true;      
    }
}