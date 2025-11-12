//
// File: Renderer.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/VulkanUtils.hpp"
#include "Types/DDSFormatTypes.hpp"
#include "Types/ECSTypes.hpp"

namespace ThatEngine
{
    bool Renderer::Init(Window *window, ResourceManager* resources, StatsTracker& statsTracker)
    {
        m_Resources = resources;
        m_StatsTracker = &statsTracker;

        // Set window
        if (!window)
        {
            THAT_CORE_ERROR("Renderer Init: Window is null!");
            return false;
        }
           
        m_Window = window;
        m_Resources = resources;
        
        m_PipelineBindOrder = { PipelineType::DefaultLit, PipelineType::WorldSpaceText, PipelineType::ScreenSpaceText, PipelineType::PostProcessing};
        SetScreenSize(window->GetInnerWidth(), window->GetInnerHeight());
        SetRenderMode(RenderMode::Color);

        // Instance
        {
            VkApplicationInfo appInfo = {};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = m_Window->GetTitle().c_str();
            appInfo.pEngineName = "ThatEngine";
            
            const char* extensions[] = {
                #ifdef PLATFORM_WINDOWS
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                #endif
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                VK_KHR_SURFACE_EXTENSION_NAME
            };
            
            std::vector<const char*> layers;

            #ifdef VULKAN_VALIDATION_LAYERS
            layers.emplace_back("VK_LAYER_KHRONOS_validation");
            #endif
            
            VkInstanceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            info.pApplicationInfo = &appInfo;
            info.ppEnabledExtensionNames = extensions;
            info.enabledExtensionCount = ARRAY_SIZE(extensions);
            info.ppEnabledLayerNames = layers.data();
            info.enabledLayerCount = layers.size();
            VK_CHECK(vkCreateInstance(&info, 0, &m_Context.Instance));
        }
        
        // Debug messenger
        {
            auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Context.Instance, "vkCreateDebugUtilsMessengerEXT");

            if (vkCreateDebugUtilsMessengerEXT)
            {
                VkDebugUtilsMessengerCreateInfoEXT info = {};
                info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
                info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
                info.pfnUserCallback = VulkanDebugCallback;
                vkCreateDebugUtilsMessengerEXT(m_Context.Instance, &info, 0, &m_Context.Debug);
            }
        }
        
        // Surface
        {
            VkWin32SurfaceCreateInfoKHR info = {};
            info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            info.hwnd = static_cast<HWND>(m_Window->GetNativeWindow());
            info.hinstance = GetModuleHandleA(0);
            VK_CHECK(vkCreateWin32SurfaceKHR(m_Context.Instance, &info, 0, &m_Context.Surface));
        }
        
        // Choose GPU
        {
            uint32_t gpuId = INVALID_UINT32_ID;
            uint32_t gpuCount = 0;
            std::array<VkPhysicalDevice, 8> gpus;
            VK_CHECK(vkEnumeratePhysicalDevices(m_Context.Instance, &gpuCount, 0));
            
            if (gpuCount == 0)
            {
                THAT_CORE_ERROR("Vulkan Init: No Vulkan-compatible GPUs found!");
                return false;
            }
            
            VK_CHECK(vkEnumeratePhysicalDevices(m_Context.Instance, &gpuCount, gpus.data()));
            
            for (uint32_t i = 0; i < gpuCount; i++)
            {
                VkPhysicalDevice gpu = gpus[i];
                
                uint32_t queueFamilyCount = 0;
                std::array<VkQueueFamilyProperties, 8> queueProperties;
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, 0);
                vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueProperties.data());
                
                for (uint32_t j = 0; j < queueFamilyCount; j++)
                {
                    if (queueProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    {
                        VkBool32 surfaceSupport = VK_FALSE;
                        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, m_Context.Surface, &surfaceSupport);
                        
                        if (surfaceSupport)
                        {
                            m_Context.Gpu = gpu;
                            gpuId = j;
                            break;
                        }
                    }
                }
            }
            
            THAT_CORE_ASSERT(gpuId != INVALID_UINT32_ID, "Vulkan Init: No GPU with a graphics-capable queue family found!", 0);

            m_Context.GpuId = gpuId;
        }
        
        // Logical device
        {
            float queuePriority = 1.0f;
            
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = m_Context.GpuId;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            
            std::array<const char*, 1> extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(m_Context.Gpu, &m_Context.GpuFeatures);

            if (m_Context.GpuFeatures.samplerAnisotropy == VK_TRUE)
            {
                THAT_CORE_INFO("Vulkan: Anisotropic filtering enabled!");
                m_Context.GpuEnabledFeatures.samplerAnisotropy = VK_TRUE;
            }

            if (m_Context.GpuFeatures.fillModeNonSolid == VK_TRUE)
            {
                THAT_CORE_INFO("Vulkan: Wireframe drawing enabled!");
                m_Context.GpuEnabledFeatures.fillModeNonSolid = VK_TRUE;
            }

            VkPhysicalDeviceVulkan12Features features12 = {};
            features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            features12.separateDepthStencilLayouts = VK_TRUE;

            VkDeviceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            info.pQueueCreateInfos = &queueInfo;
            info.queueCreateInfoCount = 1;
            info.ppEnabledExtensionNames = extensions.data();
            info.enabledExtensionCount = ARRAY_SIZE(extensions);
            info.pEnabledFeatures = &m_Context.GpuEnabledFeatures;
            info.pNext = &features12;
            VK_CHECK(vkCreateDevice(m_Context.Gpu, &info, 0, &m_Context.Device));
            
            vkGetDeviceQueue(m_Context.Device, m_Context.GpuId, 0, &m_Context.GraphicsQueue);
        }

        // Init resources that only use logical device
        {
            m_Resources->Init(&m_Context);
            m_GpuTimer.Init(m_Context.Device);
        }

        // Formats
        {
            m_Context.DepthFormat = VulkanUtils::FindSupportedFormat(m_Context.Gpu, { VK_FORMAT_D32_SFLOAT });
            m_Context.SurfaceFormat = VulkanUtils::FindSurfaceFormat(m_Context.Gpu, m_Context.Surface);
        }

        // Render Pass
        {
            // Color attachment into geometry shader
            VkAttachmentDescription colorAttachment = {};
            {
                colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
            
            // Depth attachment
            VkAttachmentDescription depthAttachment = {};
            {
                depthAttachment.format = m_Context.DepthFormat;
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

            // Color attachment for post-processing output
            VkAttachmentDescription postprocessColorAttachment = {};
            {
                postprocessColorAttachment.format = m_Context.SurfaceFormat.format;
                postprocessColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                postprocessColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                postprocessColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                postprocessColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                postprocessColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            
            // Subpass 0: Geometry
            VkAttachmentReference geometryColorAttachmentRef = {};
            {
                geometryColorAttachmentRef.attachment = 0;
                geometryColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
            }

            VkAttachmentReference geometryDepthAttachmentRef = {};
            {
                geometryDepthAttachmentRef.attachment = 1;
                geometryDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            
            VkSubpassDescription geometrySubpass = {};
            {
                geometrySubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                geometrySubpass.pColorAttachments = &geometryColorAttachmentRef;
                geometrySubpass.colorAttachmentCount = 1;
                geometrySubpass.pDepthStencilAttachment = &geometryDepthAttachmentRef;
            }
            
            // Subpass 1: Post-processing
            VkAttachmentReference postprocessInputColorAttachmentRef = {};
            {
                postprocessInputColorAttachmentRef.attachment = 0;
                postprocessInputColorAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }    
            
            VkAttachmentReference postprocessInputDepthAttachmentRef = {};
            {
                postprocessInputDepthAttachmentRef.attachment = 1;
                postprocessInputDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; 
            }

            VkAttachmentReference postprocessOutputColorAttachmentRef = {};
            {   
                postprocessOutputColorAttachmentRef.attachment = 2;
                postprocessOutputColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            std::array<VkAttachmentReference, 2> postprocessInputAttachments = { postprocessInputColorAttachmentRef, postprocessInputDepthAttachmentRef };
            VkSubpassDescription postprocessSubpass = {};
            {
                postprocessSubpass.pInputAttachments = postprocessInputAttachments.data();
                postprocessSubpass.inputAttachmentCount = static_cast<uint32_t>(postprocessInputAttachments.size());
                postprocessSubpass.pColorAttachments = &postprocessOutputColorAttachmentRef;
                postprocessSubpass.colorAttachmentCount = 1;
            }

            // Dependencies
            // Color: Geometry write -> Postprocess read
            VkSubpassDependency colorDependency = {};
            {
                colorDependency.srcSubpass = 0;
                colorDependency.dstSubpass = 1;
                colorDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                colorDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                colorDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                colorDependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                colorDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            // Depth: Geometry write -> Postprocess read
            VkSubpassDependency depthDependency = {};
            {
                depthDependency.srcSubpass = 0;
                depthDependency.dstSubpass = 1;
                depthDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                depthDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depthDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                depthDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
            
            // Arrays
            std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, postprocessColorAttachment };  
            std::array<VkSubpassDescription, 2> subpasses = { geometrySubpass, postprocessSubpass };
            std::array<VkSubpassDependency, 2> dependencies = { colorDependency, depthDependency };

            // Render Pass
            VkRenderPassCreateInfo info = {};
            {
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                info.pSubpasses = subpasses.data();
                info.subpassCount = static_cast<uint32_t>(subpasses.size());
                info.pAttachments = attachments.data();
                info.attachmentCount = static_cast<uint32_t>(attachments.size());
                info.pDependencies = dependencies.data();
                info.dependencyCount = static_cast<uint32_t>(dependencies.size());
            }
            
            VK_CHECK(vkCreateRenderPass(m_Context.Device, &info, 0, &m_Context.RenderPass));
        }
      
        // Command Pool
        {
            VkCommandPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.queueFamilyIndex = m_Context.GpuId;
            info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            VK_CHECK(vkCreateCommandPool(m_Context.Device, &info, 0, &m_Context.CommandPool));
        }

        // Sampler
        {
            VkSamplerCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.minFilter = VK_FILTER_NEAREST;
            info.magFilter = VK_FILTER_NEAREST;
            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            info.anisotropyEnable = m_Context.GpuEnabledFeatures.samplerAnisotropy;
            info.maxAnisotropy = 16.0f;
            VK_CHECK(vkCreateSampler(m_Context.Device, &info, 0, &m_Context.Sampler));
        }

        // Swapchain with image views and framebuffers
        {
            CreateSwapchain();
        }

        // Descriptor pool
        {
            std::array<VkDescriptorPoolSize, 2> poolSizes = {{
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 },
            }};

            VkDescriptorPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            info.maxSets = 16; // Pipelines x buffers (no triple buffering for now)
            info.poolSizeCount = poolSizes.size();
            info.pPoolSizes = poolSizes.data();
            VK_CHECK(vkCreateDescriptorPool(m_Context.Device, &info, 0, &m_Context.DescriptorPool));
        }

        // Buffers
        {
            m_Context.ImageStagingBuffer = m_Resources->GetBufferManager().AllocateBuffer(
                SIZE_MB(8), 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT      
            );

             m_Context.GlobalDataStagingBuffer = m_Resources->GetBufferManager().AllocateBuffer(
                256, 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT      
            );

            m_Context.GlobalDataBuffer = m_Resources->GetBufferManager().AllocateBuffer(
                256,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

            m_Context.InstanceStagingBuffer = m_Resources->GetBufferManager().AllocateBuffer(
                sizeof(MeshInstance) * ECS::MAX_ENTITIES, 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT      
            );

            m_Context.InstanceBuffer = m_Resources->GetBufferManager().AllocateBuffer(
                sizeof(MeshInstance) * ECS::MAX_ENTITIES,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );
        }

        // Late init resources that use staging buffers
        {
            m_Resources->LateInit();
        }

        // Init pipelines
        {
            m_PipelineManager.Init(&m_Context, &m_Resources->GetShaderManager());

            auto& imageManager = m_Resources->GetImageManager();

            // Bind static resources
            {
                // Default lit pipeline
                m_PipelineManager.BindBufferResource(PipelineType::DefaultLit, 0, m_Context.GlobalDataBuffer);
                m_PipelineManager.BindBufferResource(PipelineType::DefaultLit, 1, m_Context.InstanceBuffer);
                m_PipelineManager.BindImageResource(PipelineType::DefaultLit, 2, imageManager.GetTexture(TextureType::BlockWhiteTile));

                // Default lit wireframe pipeline
                m_PipelineManager.BindBufferResource(PipelineType::DefaultLitWireframe, 0, m_Context.GlobalDataBuffer);
                m_PipelineManager.BindBufferResource(PipelineType::DefaultLitWireframe, 1, m_Context.InstanceBuffer);
                m_PipelineManager.BindImageResource(PipelineType::DefaultLitWireframe, 2, imageManager.GetTexture(TextureType::BlockWhiteTile));

                // World space text pipeline
                m_PipelineManager.BindBufferResource(PipelineType::WorldSpaceText, 0, m_Context.GlobalDataBuffer);
                m_PipelineManager.BindBufferResource(PipelineType::WorldSpaceText, 1, m_Context.InstanceBuffer);
                m_PipelineManager.BindImageResource(PipelineType::WorldSpaceText, 2, imageManager.GetTexture(TextureType::DefaultFont));

                // World space text wireframe pipeline
                m_PipelineManager.BindBufferResource(PipelineType::WorldSpaceTextWireframe, 0, m_Context.GlobalDataBuffer);
                m_PipelineManager.BindBufferResource(PipelineType::WorldSpaceTextWireframe, 1, m_Context.InstanceBuffer);
                m_PipelineManager.BindImageResource(PipelineType::WorldSpaceTextWireframe, 2, imageManager.GetTexture(TextureType::DefaultFont));

                // Screen space text pipeline
                m_PipelineManager.BindBufferResource(PipelineType::ScreenSpaceText, 0, m_Context.GlobalDataBuffer);
                m_PipelineManager.BindBufferResource(PipelineType::ScreenSpaceText, 1, m_Context.InstanceBuffer);
                m_PipelineManager.BindImageResource(PipelineType::ScreenSpaceText, 2, imageManager.GetTexture(TextureType::DefaultFont));
            
                // Post-processing pipeline
                m_PipelineManager.BindBufferResource(PipelineType::PostProcessing, 0, m_Context.GlobalDataBuffer);
            }            
        }
        
        // Command Buffer
        {
            VkCommandBufferAllocateInfo allocInfo = VulkanUtils::CreateCommandBufferAllocateInfo(m_Context.CommandPool);
            VK_CHECK(vkAllocateCommandBuffers(m_Context.Device, &allocInfo, &m_Context.CommandBuffer)); 
        }
        
        // Semaphores, fences
        {
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VK_CHECK(vkCreateSemaphore(m_Context.Device, &semaphoreInfo, 0, &m_Context.AcquireSemaphore));
            VK_CHECK(vkCreateSemaphore(m_Context.Device, &semaphoreInfo, 0, &m_Context.SubmitSemaphore));
            
            m_Context.ImageAvailableFence = VulkanUtils::CreateFence(m_Context.Device, VK_FENCE_CREATE_SIGNALED_BIT);
        }

        THAT_CORE_INFO("Vulkan: Initialization is complete!");

        return true;
    }

    bool Renderer::Shutdown()
    {
        vkDeviceWaitIdle(m_Context.Device);

        vkFreeCommandBuffers(m_Context.Device, m_Context.CommandPool, 1, &m_Context.CommandBuffer);
        vkDestroyCommandPool(m_Context.Device, m_Context.CommandPool, 0);   
        vkDestroyRenderPass(m_Context.Device, m_Context.RenderPass, 0);
        vkDestroyDescriptorPool(m_Context.Device, m_Context.DescriptorPool, 0);
        vkDestroySampler(m_Context.Device, m_Context.Sampler, 0);
        
        // Swapchain
        DestroySwapchain();
        vkDestroySurfaceKHR(m_Context.Instance, m_Context.Surface, 0);

        // Pipelines
        m_PipelineManager.Shutdown();

        // Buffers
        m_Resources->GetBufferManager().DestroyBuffer(m_Context.ImageStagingBuffer);
        m_Resources->GetBufferManager().DestroyBuffer(m_Context.GlobalDataStagingBuffer);
        m_Resources->GetBufferManager().DestroyBuffer(m_Context.GlobalDataBuffer);
        m_Resources->GetBufferManager().DestroyBuffer(m_Context.InstanceStagingBuffer);
        m_Resources->GetBufferManager().DestroyBuffer(m_Context.InstanceBuffer);

        // Resource manager
        m_Resources->Shutdown();

        // Gpu timer
        m_GpuTimer.Shutdown();

        // Async
        vkWaitForFences(m_Context.Device, 1, &m_Context.ImageAvailableFence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(m_Context.Device, m_Context.ImageAvailableFence, 0);
        vkDestroySemaphore(m_Context.Device, m_Context.AcquireSemaphore, 0);
        vkDestroySemaphore(m_Context.Device, m_Context.SubmitSemaphore, 0);

        // Device
        vkDestroyDevice(m_Context.Device, 0);
        return true;
    }

    void Renderer::SetRenderMode(RenderMode mode)
    {
        // Switch to wireframe related pipelines
        if (mode == RenderMode::Wireframe)
        {
            // Feature to draw lines must be enabled
            if (m_Context.GpuEnabledFeatures.fillModeNonSolid == VK_FALSE) return;

            m_PipelineBindOrder[0] = PipelineType::DefaultLitWireframe;
            m_PipelineBindOrder[1] = PipelineType::WorldSpaceTextWireframe;
        }

        else
        {
            m_PipelineBindOrder[0] = PipelineType::DefaultLit;
            m_PipelineBindOrder[1] = PipelineType::WorldSpaceText;
        }

        // Update active pipelines
        m_PipelineManager.SetActivePipelines(m_PipelineBindOrder);

        m_RenderMode = mode;
        THAT_CORE_INFO("Renderer: {}_MODE enabled!", ToString(mode));
    }
    
    void Renderer::OnWindowResize(WindowResizeEvent& event)
    {
        uint32_t width = event.GetWidth();
        uint32_t height = event.GetHeight();

        if (width == 0 || height == 0) return;

        SetScreenSize(width, height);
    }

    void Renderer::UploadRenderableDatapackToGPU(VkCommandBuffer& cmd, RenderableDatapack& datapack)
    {
        VkDeviceSize offset = 0;
        VkDeviceSize totalDataSize = 0;

        UploadInstanceBatches<MeshAssetType, MeshInstance>(cmd, datapack.MeshInstanceBatches, totalDataSize, datapack.MeshInstanceBatchesOffset, offset);
        UploadInstanceBatches<FontAssetType, GlyphInstance>(cmd, datapack.WorldSpaceGlyphInstanceBatches, totalDataSize, datapack.WorldSpaceGlyphInstanceBatchesOffset, offset);
        UploadInstanceBatches<FontAssetType, GlyphInstance>(cmd, datapack.ScreenSpaceGlyphInstanceBatches, totalDataSize, datapack.ScreenSpaceGlyphInstanceBatchesOffset, offset);
        m_Resources->GetBufferManager().CopyData(cmd, m_Context.InstanceStagingBuffer, m_Context.InstanceBuffer, totalDataSize, 0, 0);

        // Update buffer offsets
        m_PipelineManager.UpdatePipelineBoundBufferOffset(m_PipelineBindOrder[1], 1, datapack.WorldSpaceGlyphInstanceBatchesOffset);
        m_PipelineManager.UpdatePipelineBoundBufferOffset(PipelineType::ScreenSpaceText, 1, datapack.ScreenSpaceGlyphInstanceBatchesOffset);
    }

    void Renderer::UploadGlobalData(const GlobalData& globalData)
    {
        VkCommandBuffer cmd = VulkanUtils::BeginSingleTimeCommands(m_Context.Device, m_Context.CommandPool);

        m_Resources->GetBufferManager().UploadData(m_Context.GlobalDataStagingBuffer, &globalData, m_Context.GlobalDataBuffer.Size, 0);
        m_Resources->GetBufferManager().CopyData(cmd, m_Context.GlobalDataStagingBuffer, m_Context.GlobalDataBuffer, m_Context.GlobalDataBuffer.Size, 0, 0);

        VulkanUtils::EndSingleTimeCommands(m_Context.Device, m_Context.GraphicsQueue, m_Context.CommandPool, cmd);
    }

    void Renderer::UpdateViewport(const VkCommandBuffer& cmd)
    {   
        vkCmdSetViewport(cmd, 0, 1, &m_Context.Viewport);
        vkCmdSetScissor(cmd, 0, 1, &m_Context.Scissor);
    }

    bool Renderer::Render(RenderableDatapack& datapack)
    {
        if (m_Window->IsMinimized())
        {
            return false;
        }
        
        if (m_RecreateSwapchain)
        {
            RecreateSwapchain();
        }

        if(!BeginFrame())
        {
            return false;
        }

        RecordCommands(datapack);

        EndFrame();

        return true; 
    }

    bool Renderer::BeginFrame()
    {   
        VK_CHECK(vkWaitForFences(m_Context.Device, 1, &m_Context.ImageAvailableFence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(m_Context.Device, 1, &m_Context.ImageAvailableFence));
        VkResult result = vkAcquireNextImageKHR(m_Context.Device, m_Context.Swapchain, UINT64_MAX, m_Context.AcquireSemaphore, 0, &m_CurrentImageId);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_RecreateSwapchain = true;
            return false;
        }  
        
        VK_CHECK(vkResetCommandBuffer(m_Context.CommandBuffer, 0));
        
        return true;
    }

    void Renderer::RecordCommands(RenderableDatapack& datapack)
    {
        VkCommandBuffer cmd = m_Context.CommandBuffer;
        VkCommandBufferBeginInfo info = VulkanUtils::CreateCommandBufferBeginInfo();
        VK_CHECK(vkBeginCommandBuffer(cmd, &info));

        m_GpuTimer.StartTimestamp(m_Context.CommandBuffer);

        // Upload datapack to GPU
        {
            UploadRenderableDatapackToGPU(cmd, datapack);
        }

        // Set active pipelines and update their descriptor sets before starting render pass
        {
            m_PipelineManager.BindInputAttachmentResource(PipelineType::PostProcessing, 3, m_Context.GeometryColorImages[m_CurrentImageId], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_PipelineManager.BindInputAttachmentResource(PipelineType::PostProcessing, 4, m_Context.DepthImages[m_CurrentImageId], VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

            m_PipelineManager.UpdateDescriptorSets();
        }

        // Begin render pass
        {
            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = { datapack.ClearColor.r, datapack.ClearColor.g, datapack.ClearColor.b, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = m_Context.RenderPass;
            info.renderArea.extent = m_Context.ScreenSize;
            info.framebuffer = m_Context.Framebuffers[m_CurrentImageId];
            info.pClearValues = clearValues.data();
            info.clearValueCount = clearValues.size();
            vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Subpass 0: Geometry with text
        {
            UpdateViewport(cmd);

            // Default lit pipeline
            m_PipelineManager.BindPipeline(cmd, m_PipelineBindOrder[0]);

            // Meshes
            for (const auto& [mesh, batch] : datapack.MeshInstanceBatches)
            {
                if (batch.Instances.empty()) continue;
                
                VkDeviceSize offset = 0;
                // Bind vertex and index data for the current mesh
                const MeshGPUData& meshGpuData = m_Resources->GetMeshManager().GetMeshAssetGPUData(mesh);
                vkCmdBindVertexBuffers(cmd, 0, 1, &meshGpuData.VertexBuffer.Buffer, &offset);
                vkCmdBindIndexBuffer(cmd, meshGpuData.IndexBuffer.Buffer, offset, VK_INDEX_TYPE_UINT32);

                // One draw call per mesh type
                vkCmdDrawIndexed(cmd, meshGpuData.IndexCount, static_cast<uint32_t>(batch.Instances.size()), 0, 0, batch.FirstInstance);
            }

            // Prepare quad mesh for text rendering
            VkDeviceSize offset = 0;
            const MeshGPUData& quadMeshData = m_Resources->GetMeshManager().GetMeshAssetGPUData(MeshAssetType::Quad);
            vkCmdBindVertexBuffers(cmd, 0, 1, &quadMeshData.VertexBuffer.Buffer, &offset);
            vkCmdBindIndexBuffer(cmd, quadMeshData.IndexBuffer.Buffer, offset, VK_INDEX_TYPE_UINT32);

            // World space text pipeline
            m_PipelineManager.BindPipeline(cmd, m_PipelineBindOrder[1]);

            for (const auto& [font, batch] : datapack.WorldSpaceGlyphInstanceBatches)
            {
                if (batch.Instances.empty()) continue;
                
                vkCmdDrawIndexed(cmd, quadMeshData.IndexCount, static_cast<uint32_t>(batch.Instances.size()), 0, 0, batch.FirstInstance);
            }     
            
            // Screen space text pipeline
            m_PipelineManager.BindPipeline(cmd, m_PipelineBindOrder[2]);

            for (const auto& [font, batch] : datapack.ScreenSpaceGlyphInstanceBatches)
            {
                if (batch.Instances.empty()) continue;
                
                vkCmdDrawIndexed(cmd, quadMeshData.IndexCount, static_cast<uint32_t>(batch.Instances.size()), 0, 0, batch.FirstInstance);
            }
        }

        vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);

        // Subpass 1: Post-processing
        {
            UpdateViewport(cmd);
            m_PipelineManager.BindPipeline(cmd, m_PipelineBindOrder[3]);

            // Screen quad is baked into the shader
            vkCmdDraw(cmd, 6, 1, 0, 0);
        }
    }

    void Renderer::EndFrame()
    {
        VkCommandBuffer cmd = m_Context.CommandBuffer;
        vkCmdEndRenderPass(cmd);
        m_GpuTimer.EndTimestamp(cmd);
        VK_CHECK(vkEndCommandBuffer(cmd));

        // Submit
        {
            VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = VulkanUtils::CreateSubmitInfo(&cmd);
            info.pWaitDstStageMask = &stageMask;
            info.pWaitSemaphores = &m_Context.AcquireSemaphore;
            info.waitSemaphoreCount = 1;
            info.pSignalSemaphores = &m_Context.SubmitSemaphore;
            info.signalSemaphoreCount = 1;
            VK_CHECK(vkQueueSubmit(m_Context.GraphicsQueue, 1, &info, m_Context.ImageAvailableFence));
        }

        // Present
        {
            VkPresentInfoKHR info = {};
            info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.pSwapchains = &m_Context.Swapchain;
            info.swapchainCount = 1;
            info.pImageIndices = &m_CurrentImageId;
            info.pWaitSemaphores = &m_Context.SubmitSemaphore;
            info.waitSemaphoreCount = 1;
            VkResult result = vkQueuePresentKHR(m_Context.GraphicsQueue, &info);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                m_RecreateSwapchain = true;
            }    
        }   

        m_StatsTracker->SetGpuTime(m_GpuTimer.GetElapsedTimeMs());
    }

    void Renderer::CreateSwapchain()
    {
        VkSurfaceCapabilitiesKHR capabilities = {};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Context.Gpu, m_Context.Surface, &capabilities));            
        
        const uint32_t width = capabilities.currentExtent.width;
        const uint32_t height = capabilities.currentExtent.height;

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.surface = m_Context.Surface;
        info.preTransform = capabilities.currentTransform;
        info.imageFormat = m_Context.SurfaceFormat.format;
        info.imageExtent = capabilities.currentExtent;
        uint32_t minImageCount = capabilities.minImageCount + 1 > capabilities.maxImageCount ? capabilities.minImageCount : capabilities.minImageCount + 1;
        info.minImageCount = minImageCount;
        info.imageArrayLayers = 1;
        VK_CHECK(vkCreateSwapchainKHR(m_Context.Device, &info, 0, &m_Context.Swapchain));
        
        // Create framebuffers
        {
            VkFramebufferCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.renderPass = m_Context.RenderPass;
            info.width = m_Context.ScreenSize.width;
            info.height = m_Context.ScreenSize.height;
            info.layers = 1;

            std::array<VkImage,VkContext::MAX_SWAPCHAIN_IMAGES> swapchainImageHandles;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Context.Device, m_Context.Swapchain, &m_Context.SwapchainImageCount, 0));
            VK_CHECK(vkGetSwapchainImagesKHR(m_Context.Device, m_Context.Swapchain, &m_Context.SwapchainImageCount, swapchainImageHandles.data())); 
            
            auto& imageManager = m_Resources->GetImageManager();

            for (uint32_t i = 0; i < m_Context.SwapchainImageCount; i++)
            {
                // Geometry color images
                m_Context.GeometryColorImages[i] = imageManager.AllocateImage(width, height, 1, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);        
                imageManager.CreateImageView(m_Context.GeometryColorImages[i], VK_IMAGE_ASPECT_COLOR_BIT);
                
                // Depth images
                m_Context.DepthImages[i] = imageManager.AllocateImage(width, height, 1, m_Context.DepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT );        
                imageManager.CreateImageView(m_Context.DepthImages[i], VK_IMAGE_ASPECT_DEPTH_BIT);
                
                // Swapchain images
                m_Context.SwapchainImages[i] = imageManager.AllocateImage(width, height, 1, m_Context.SurfaceFormat.format, 0, swapchainImageHandles[i]);
                imageManager.CreateImageView(m_Context.SwapchainImages[i], VK_IMAGE_ASPECT_COLOR_BIT);

                // Framebuffer
                std::array<VkImageView, 3> attachments = { m_Context.GeometryColorImages[i]->View, m_Context.DepthImages[i]->View, m_Context.SwapchainImages[i]->View };
                info.pAttachments = attachments.data();
                info.attachmentCount = static_cast<uint32_t>(attachments.size());
                VK_CHECK(vkCreateFramebuffer(m_Context.Device, &info, 0, &m_Context.Framebuffers[i]));
            }
        }

        m_RecreateSwapchain = false;
    }

    void Renderer::DestroySwapchain()
    {
        auto& imageManager = m_Resources->GetImageManager();

        for (uint32_t i = 0; i < m_Context.SwapchainImageCount; i++)
        {
            imageManager.DestroyImage(m_Context.GeometryColorImages[i]);
            imageManager.DestroyImage(m_Context.DepthImages[i]);
            vkDestroyImageView(m_Context.Device, m_Context.SwapchainImages[i]->View, nullptr);
            vkDestroyFramebuffer(m_Context.Device, m_Context.Framebuffers[i], nullptr);
        }
        
        vkDestroySwapchainKHR(m_Context.Device, m_Context.Swapchain, 0);
    }

    void Renderer::RecreateSwapchain()
    {
        VK_CHECK(vkDeviceWaitIdle(m_Context.Device));

        DestroySwapchain();
        CreateSwapchain();
    }

    void Renderer::SetScreenSize(uint32_t width, uint32_t height)
    {
        m_Context.ScreenSize.width = width;
        m_Context.ScreenSize.height = height;

        m_Context.Viewport.x = 0.0f;
        m_Context.Viewport.y = (float)m_Context.ScreenSize.height;
        m_Context.Viewport.width = (float)m_Context.ScreenSize.width;
        m_Context.Viewport.height = -(float)m_Context.ScreenSize.height;
        m_Context.Viewport.minDepth = 0.0f;
        m_Context.Viewport.maxDepth = 1.0f;

        m_Context.Scissor.offset.x = 0;
        m_Context.Scissor.offset.y = 0;
        m_Context.Scissor.extent = m_Context.ScreenSize;

        m_RecreateSwapchain = true;
    }
}