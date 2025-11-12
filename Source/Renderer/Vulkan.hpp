//
// File: Vulkan.hpp
// Description: Defines Vulkan-related macros, Vulkan debug messenger callback
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Core/Memory.hpp"

#include <vulkan/vulkan.hpp>

namespace ThatEngine
{
    #define VK_CHECK(result)                                                \
    do                                                                      \
    {                                                                       \
        if (result != VK_SUCCESS)                                           \
        {                                                                   \
            THAT_CORE_ERROR("Vulkan Error: {}", string_VkResult(result));   \
            __debugbreak();                                                 \
        }                                                                   \
    }                                                                       \
    while(0)

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
        VkDebugUtilsMessageTypeFlagsEXT msgType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        if (msgSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            THAT_CORE_ERROR(pCallbackData->pMessage);
        }
        else if (msgSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            THAT_CORE_WARN(pCallbackData->pMessage);
        }
        else
        {
            THAT_CORE_INFO(pCallbackData->pMessage);
        }
        return false;
    }
}
    