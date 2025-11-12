//
// File: PipelineManager.hpp
// Description: Manages Vulkan Graphics pipelines
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/GraphicsContext.hpp"
#include "Renderer/ShaderManager.hpp"
#include "Types/PipelineTypes.hpp"

#include <unordered_map>

namespace ThatEngine
{
    class PipelineManager
    {
        public:
        PipelineManager() = default;
        void Init(VkContext* context, ShaderManager* shaderManager);
        void Shutdown();

        void PrepareResourceBinding(PipelineType type, uint32_t binding, BoundResourceType resourceType, VkDescriptorType descriptorType);
        void BindPipeline(const VkCommandBuffer& cmd, PipelineType type);
        void BindImageResource(PipelineType type, uint32_t binding, const Shared<Image>& image);
        void BindInputAttachmentResource(PipelineType type, uint32_t binding, const Shared<Image>& image, VkImageLayout imageLayout);
        void BindBufferResource(PipelineType type, uint32_t binding, Buffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
        void UpdateDescriptorSets();
        void UpdatePipelineBoundBufferOffset(PipelineType type, uint32_t binding, VkDeviceSize offset);

        template<PipelineType... Types>
        void SetActivePipelines()
        {
            m_ActivePipelines.reset();
            (..., m_ActivePipelines.set(static_cast<uint32_t>(Types)));
        }

        template <std::uint32_t N>
        void SetActivePipelines(const std::array<PipelineType, N> types)
        {
            m_ActivePipelines.reset();
            for (std::uint32_t i = 0; i < N; i++)
            {
                m_ActivePipelines.set(static_cast<uint32_t>(types[i]));
            }
        }

        private:
        bool CreatePipeline(const PipelineCreateInfo& info);

        private:
        std::unordered_map<PipelineType, Shared<PipelineResources>> m_Pipelines;
        std::bitset<static_cast<uint32_t>(PipelineType::Count)> m_ActivePipelines;
        PipelineType m_CurrentBoundPipelineType;

        VkContext* m_Context;
        ShaderManager* m_ShaderManager;
    };
}