//
// File: VulkanUtils.hpp
// Description: Utility functions for Vulkan
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace ThatEngine
{
    namespace VulkanUtils
    {
        VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags = 0);
        VkSubmitInfo CreateSubmitInfo(VkCommandBuffer* commandBuffer, uint32_t commandCount = 1);
        VkCommandBufferBeginInfo CreateCommandBufferBeginInfo();
        VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo(VkCommandPool commandPool);

        VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
        void EndSingleTimeCommands(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);

        uint32_t GetMemoryTypeIndex(VkPhysicalDevice gpu, VkMemoryRequirements memoryReqs, VkMemoryPropertyFlags memoryPropFlags);
        uint32_t GetFormatSize(VkFormat format);

        void MergeDescriptorBindings(std::vector<VkDescriptorSetLayoutBinding>& first, const std::vector<VkDescriptorSetLayoutBinding>& second, VkShaderStageFlagBits stage);
        void MergePushConstantRanges(std::vector<VkPushConstantRange>& first, const std::vector<VkPushConstantRange>& second, VkShaderStageFlagBits stage);
        VkFormat FindSupportedFormat(VkPhysicalDevice gpu, const std::vector<VkFormat>& candidates);
        VkSurfaceFormatKHR FindSurfaceFormat(VkPhysicalDevice gpu, VkSurfaceKHR surface);
    }
}