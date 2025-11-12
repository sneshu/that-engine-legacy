//
// File: Renderer.hpp
// Description: Responsible for initializing Vulkan, recreating swapchain, sending commands to GPU,
//              owns all render-related managers
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Window.hpp"
#include "Core/StatsTracker.hpp"
#include "Core/Event/WindowEvent.hpp"
#include "Renderer/Vulkan.hpp"
#include "Renderer/ResourceManager.hpp"
#include "Renderer/PipelineManager.hpp"
#include "Renderer/GpuTimer.hpp"
#include "Types/RendererTypes.hpp"

namespace ThatEngine
{
    class Renderer
    {
        public:
        Renderer() = default;
        bool Init(Window* window, ResourceManager* resources, StatsTracker& statsTracker);
        bool Shutdown();
        
        inline const VkContext& GetGraphicsContext() const { return m_Context; }
        inline const RenderMode& GetRenderMode() const { return m_RenderMode; }

        void SetRenderMode(RenderMode mode);

        void OnWindowResize(WindowResizeEvent& event);

        void UploadRenderableDatapackToGPU(VkCommandBuffer& cmd, RenderableDatapack& datapack);
        void UploadGlobalData(const GlobalData& globalData);
        bool Render(RenderableDatapack& datapack);
        
        private:
        bool BeginFrame();
        void RecordCommands(RenderableDatapack& datapack);
        void EndFrame();
        void CreateSwapchain();
        void DestroySwapchain();
        void RecreateSwapchain();
        void SetScreenSize(uint32_t width, uint32_t height);
        void UpdateViewport(const VkCommandBuffer& cmd);

        template<typename AssetType, typename InstanceType>
        void UploadInstanceBatches(VkCommandBuffer& cmd, std::unordered_map<AssetType, InstanceBatch<InstanceType>>& batches, VkDeviceSize& totalDataSize, VkDeviceSize& batchOffset, VkDeviceSize& offset)
        {
            batchOffset = offset;
            for (auto& [_, batch] : batches)
            {
                if (batch.Instances.empty()) continue;

                VkDeviceSize dataSize = sizeof(InstanceType) * batch.Instances.size();
                m_Resources->GetBufferManager().UploadData(m_Context.InstanceStagingBuffer, batch.Instances.data(), dataSize, offset);
                
                batch.FirstInstance = static_cast<uint32_t>((offset - batchOffset) / sizeof(InstanceType));
                
                offset += dataSize;
                totalDataSize += dataSize;
            }
        }
        
        private:
        VkContext m_Context;
        Window* m_Window;
        ResourceManager* m_Resources;
        StatsTracker* m_StatsTracker;
        PipelineManager m_PipelineManager;
        GpuTimer m_GpuTimer;
        
        std::array<PipelineType, 4> m_PipelineBindOrder;
        RenderMode m_RenderMode;
        bool m_RecreateSwapchain;
        uint32_t m_CurrentImageId = 0;
    };
}