//
// File: GraphicsContext.hpp
// Description: Defines core Vulkan rendering context that manages GPU state,
//              swapchain, render targets and synchronization objects
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Types/PipelineTypes.hpp"
#include "Types/ImageTypes.hpp"
#include "Types/BufferTypes.hpp"

namespace ThatEngine
{
    struct VkContext
    {
        VkInstance Instance;
        VkSurfaceKHR Surface;
        VkSurfaceFormatKHR SurfaceFormat;
        VkFormat DepthFormat;
        VkPhysicalDevice Gpu;
        VkPhysicalDeviceFeatures GpuFeatures;
        VkPhysicalDeviceFeatures GpuEnabledFeatures;
        uint32_t GpuId;
        VkDevice Device;
        VkSwapchainKHR Swapchain;
        VkRenderPass RenderPass;

        Shared<PipelineResources> ActivePipeline;        
        VkSampler Sampler;
        VkDescriptorPool DescriptorPool;
        
        static constexpr uint32_t MAX_SWAPCHAIN_IMAGES = 4;
        uint32_t SwapchainImageCount;

        Shared<Image> GeometryColorImages[VkContext::MAX_SWAPCHAIN_IMAGES];
        Shared<Image> DepthImages[VkContext::MAX_SWAPCHAIN_IMAGES];
        Shared<Image> SwapchainImages[VkContext::MAX_SWAPCHAIN_IMAGES];
        VkFramebuffer Framebuffers[VkContext::MAX_SWAPCHAIN_IMAGES];
        VkQueue GraphicsQueue;
        VkCommandBuffer CommandBuffer;
        VkCommandPool CommandPool;
        
        VkSemaphore AcquireSemaphore;
        VkSemaphore SubmitSemaphore;
        VkFence ImageAvailableFence;
        
        VkDebugUtilsMessengerEXT Debug;
        VkExtent2D ScreenSize;
        VkViewport Viewport;
        VkRect2D Scissor;
        
        Buffer ImageStagingBuffer;
        Buffer GlobalDataBuffer;
        Buffer GlobalDataStagingBuffer;
        Buffer InstanceBuffer;
        Buffer InstanceStagingBuffer;
    };
}