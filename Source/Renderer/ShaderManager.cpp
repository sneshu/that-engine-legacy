//
// File: ShaderManager.cpp
//
// Copyright (c) 2025 Sneshu
// All rights reserved.
//

#include "Core/PCH.hpp"
#include "Renderer/ShaderManager.hpp"
#include "Renderer/VulkanUtils.hpp"

namespace ThatEngine
{
    void ShaderManager::Init(VkContext* context)
    {
        m_Context = context;
    }

    void ShaderManager::Shutdown()
    {
        for (auto& [stage, shaderModuleMap] : m_ShaderModules)
        {
            for (auto& [name, shaderModule] : shaderModuleMap)
            {
                if (shaderModule && shaderModule->Handle != VK_NULL_HANDLE)
                {
                    vkDestroyShaderModule(m_Context->Device, shaderModule->Handle, 0);
                    shaderModule->Handle = VK_NULL_HANDLE;
                }
            }
        }

        m_ShaderModules.clear();
    }

    const Shared<ShaderProgram>& ShaderManager::CreateProgram(ShaderProgramType type, const std::string& vertexPath, const std::string& fragmentPath)
    {
        Shared<ShaderModule> vertexShader = LoadShader(vertexPath, VK_SHADER_STAGE_VERTEX_BIT);

        if (!vertexShader)
        {
            THAT_CORE_ERROR("Failed to create or load vertex shader: {}", vertexPath);
        }

        Shared<ShaderModule> fragmentShader = LoadShader(fragmentPath, VK_SHADER_STAGE_FRAGMENT_BIT);

        if (!fragmentShader)
        {
            THAT_CORE_ERROR("Failed to create or load fragment shader: {}", fragmentPath);
        }

        Shared<ShaderProgram> shaderProgram = CreateShared<ShaderProgram>();
        shaderProgram->Vertex = vertexShader;
        shaderProgram->Fragment = fragmentShader;

        // Descriptor bindings merge and sorting
        shaderProgram->MergedDescriptorBindings.clear();
        VulkanUtils::MergeDescriptorBindings(shaderProgram->MergedDescriptorBindings, vertexShader->DescriptorBindings, VK_SHADER_STAGE_VERTEX_BIT);
        VulkanUtils::MergeDescriptorBindings(shaderProgram->MergedDescriptorBindings, fragmentShader->DescriptorBindings, VK_SHADER_STAGE_FRAGMENT_BIT);
        std::ranges::sort(shaderProgram->MergedDescriptorBindings, {}, &VkDescriptorSetLayoutBinding::binding);

        // Push constant ranges merge
        shaderProgram->MergedPushConstantRanges.clear();
        VulkanUtils::MergePushConstantRanges(shaderProgram->MergedPushConstantRanges, vertexShader->PushConstantRanges, VK_SHADER_STAGE_VERTEX_BIT);
        VulkanUtils::MergePushConstantRanges(shaderProgram->MergedPushConstantRanges, fragmentShader->PushConstantRanges, VK_SHADER_STAGE_FRAGMENT_BIT);

        m_ShaderPrograms[type] = shaderProgram;

        return m_ShaderPrograms[type];
    }

    Shared<ShaderModule> ShaderManager::LoadShader(const std::string& path, VkShaderStageFlagBits stage)
    {
        std::string name = FileReader::GetFileName(path);
        auto& stageMap = m_ShaderModules[stage];

        // Find cached shader module
        {
            auto iterator = stageMap.find(name);
            if (iterator != stageMap.end())
            {
                return iterator->second;
            }
        }
        
        // Create shader module
        {
            uint32_t shaderSize = 0;
            auto shaderData = FileReader::ReadBinaryFile<uint32_t>(path, shaderSize);

            THAT_CORE_ASSERT(shaderData != nullptr, "Failed to load shader: {}", path);
            
            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = shaderSize;
            createInfo.pCode = shaderData;
            
            VkShaderModule vkShaderModule;
            VK_CHECK(vkCreateShaderModule(m_Context->Device, &createInfo, nullptr, &vkShaderModule));
            
            Shared<ShaderModule> shaderModule = CreateShared<ShaderModule>(
                ShaderModule 
                { 
                    .Stage = stage, 
                    .Handle = vkShaderModule
                }
            );

            // Reflection
            SpvReflectShaderModule reflection;
            {
                SPV_CHECK(spvReflectCreateShaderModule(shaderSize, shaderData, &reflection));
                delete[] shaderData;

                // Entry point
                shaderModule->EntryPoint = reflection.entry_point_name;

                // Descriptor bindings
                {
                    uint32_t bindingCount = 0;
                    SPV_CHECK(spvReflectEnumerateDescriptorBindings(&reflection, &bindingCount, nullptr));
                    std::vector<SpvReflectDescriptorBinding*> reflectedBindings(bindingCount);
                    SPV_CHECK(spvReflectEnumerateDescriptorBindings(&reflection, &bindingCount, reflectedBindings.data()));

                    for (SpvReflectDescriptorBinding* reflectedBinding : reflectedBindings)
                    {
                        VkDescriptorSetLayoutBinding layoutBinding = {};
                        layoutBinding.binding = reflectedBinding->binding;
                        layoutBinding.descriptorType = static_cast<VkDescriptorType>(reflectedBinding->descriptor_type);
                        layoutBinding.descriptorCount = reflectedBinding->count;
                        layoutBinding.stageFlags = stage;

                        shaderModule->DescriptorBindings.emplace_back(layoutBinding);
                    }   
                }

                // Push constants
                {
                    uint32_t pushConstantCount = 0;
                    SPV_CHECK(spvReflectEnumeratePushConstantBlocks(&reflection, &pushConstantCount, nullptr));
                    std::vector<SpvReflectBlockVariable*> reflectedPushConstants(pushConstantCount);
                    SPV_CHECK(spvReflectEnumeratePushConstantBlocks(&reflection, &pushConstantCount, reflectedPushConstants.data()));

                    for (SpvReflectBlockVariable* reflectedPushConstant : reflectedPushConstants)
                    {
                        VkPushConstantRange range = {};
                        range.offset = reflectedPushConstant->offset;
                        range.size = reflectedPushConstant->size;
                        range.stageFlags = stage;

                        shaderModule->PushConstantRanges.emplace_back(range);
                    }
                }

                // Vertex inputs
                if (stage == VK_SHADER_STAGE_VERTEX_BIT)
                {
                    uint32_t inputCount = 0;
                    SPV_CHECK(spvReflectEnumerateInputVariables(&reflection, &inputCount, nullptr));
                    std::vector<SpvReflectInterfaceVariable*> reflectedInputs(inputCount);
                    SPV_CHECK(spvReflectEnumerateInputVariables(&reflection, &inputCount, reflectedInputs.data()));

                    // Sort inputs by location
                    std::ranges::sort(reflectedInputs, {}, &SpvReflectInterfaceVariable::location);

                    uint32_t offset = 0;
                    for (SpvReflectInterfaceVariable* reflectedInput : reflectedInputs)
                    {
                        if (reflectedInput->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) continue;

                        VkFormat format = static_cast<VkFormat>(reflectedInput->format);
                        uint32_t formatSize = VulkanUtils::GetFormatSize(format);

                        VkVertexInputAttributeDescription attribute = {};
                        attribute.location = reflectedInput->location;
                        attribute.binding = 0;
                        attribute.format = format;
                        attribute.offset = offset;

                        shaderModule->VertexAttributeDescriptions.emplace_back(attribute);
                        offset += formatSize;
                    }

                    VkVertexInputBindingDescription vertexBinding = {};
                    vertexBinding.binding = 0;
                    vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                    vertexBinding.stride = offset;
                    shaderModule->VertexBindingDescriptions.emplace_back(vertexBinding);
                }

                spvReflectDestroyShaderModule(&reflection);
            }

            THAT_CORE_INFO("Shader Manager: Loading Asset \"{}\"", path);

            stageMap[name] = shaderModule;
            return shaderModule;
        }
    }
}