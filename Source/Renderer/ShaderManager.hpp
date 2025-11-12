//
// File: ShaderManager.hpp
// Description: Manages SPIR-V shader modules, creates shader programs via reflection
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#pragma once

#include "Renderer/Vulkan.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Types/ShaderTypes.hpp"

#include <spirv_reflect.h>

namespace ThatEngine
{
    inline const char* string_SpvReflectResult(SpvReflectResult result)
    {
        switch (result)
        {
            case SPV_REFLECT_RESULT_SUCCESS: return "Success";
            case SPV_REFLECT_RESULT_NOT_READY: return "Not Ready";
            case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED: return "Parse Failed";
            case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED: return "Alloc Failed";
            case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED: return "Range Exceeded";
            case SPV_REFLECT_RESULT_ERROR_NULL_POINTER: return "Null Pointer";
            case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR: return "Internal Error";
            case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH: return "Count Mismatch";
            case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND: return "Element Not Found";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE: return "Invalid Code Size";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER: return "Invalid Magic Number";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF: return "Unexpected EOF";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE: return "Invalid ID Reference";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW: return "Set Number Overflow";
            case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS: return "Invalid Storage Class";
            default: return "Unknown Error";
        }
    }

    #define SPV_CHECK(result)                                                               \
    do                                                                                      \
    {                                                                                       \
        if (result != SPV_REFLECT_RESULT_SUCCESS)                                           \
        {                                                                                   \
            THAT_CORE_ERROR("SPIRV Reflect Error: {}", string_SpvReflectResult(result));    \
            __debugbreak();                                                                 \
        }                                                                                   \
    }                                                                                       \
    while(0)

    class ShaderManager
    {
        public:
        ShaderManager() = default;
        ~ShaderManager() = default;
        void Init(VkContext* context);
        void Shutdown();

        const Shared<ShaderProgram>& CreateProgram(ShaderProgramType type, const std::string& vertexPath, const std::string& fragmentPath);
        inline const Shared<ShaderProgram>& GetProgram(ShaderProgramType type) const { return m_ShaderPrograms.at(type); }

        private:
        VkContext* m_Context;
        Shared<ShaderModule> LoadShader(const std::string& path, VkShaderStageFlagBits stage);
        std::unordered_map<VkShaderStageFlagBits, std::unordered_map<std::string, Shared<ShaderModule>>> m_ShaderModules;
        std::unordered_map<ShaderProgramType, Shared<ShaderProgram>> m_ShaderPrograms;
    };
}