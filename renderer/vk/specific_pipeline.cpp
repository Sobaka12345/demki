#include "specific_pipeline.hpp"

#include <spirv_reflect.h>

namespace renderer::vk {

[[nodiscard]] inline constexpr VkDescriptorType toDescriptorType(SpvReflectDescriptorType type)
{
    switch (type)
    {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        default: ASSERT(false, "unknown descriptor type");
    };

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

[[nodiscard]] inline constexpr VkShaderStageFlagBits toShaderStageFlagBits(
    SpvReflectShaderStageFlagBits stage)
{
    switch (stage)
    {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: return VK_SHADER_STAGE_VERTEX_BIT;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT: return VK_SHADER_STAGE_COMPUTE_BIT;
        case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV: return VK_SHADER_STAGE_TASK_BIT_NV;
        case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV: return VK_SHADER_STAGE_MESH_BIT_NV;
        case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR: return VK_SHADER_STAGE_MISS_BIT_KHR;
        case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
        default: ASSERT(false, "unknown shader stage");
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

using namespace handles;

ISpecificPipeline::~ISpecificPipeline()
{
    PipelineLayout::destroy(m_context.device(), m_pipelineLayout, nullptr);
    m_setLayouts.destroyAll(m_context.device());
    m_shaderModules.destroyAll(m_context.device());
}

void ISpecificPipeline::initModules(
    std::span<const std::shared_ptr<SpvReflectShaderModule>> modules)
{
    m_shaderStageCreateInfos.reserve(modules.size());

    for (auto& module : modules)
    {
        const auto shaderStageFlag = toShaderStageFlagBits(module->shader_stage);
        for (uint32_t i = 0; i < module->descriptor_set_count; ++i)
        {
            const auto& spirvSet = module->descriptor_sets[i];
            std::vector<DescriptorSetLayoutBinding> bindings{ spirvSet.binding_count };
            for (uint32_t j = 0; j < spirvSet.binding_count; ++j)
            {
                const auto& spirvBinding = spirvSet.bindings[j];
                bindings[j]
                    .binding(spirvBinding->binding)
                    .descriptorCount(spirvBinding->count)
                    .descriptorType(toDescriptorType(spirvBinding->descriptor_type))
                    .stageFlags(shaderStageFlag);
            }

            const auto createInfo =
                DescriptorSetLayoutCreateInfo{}
                    .bindingCount(bindings.size())
                    .pBindings(bindings.data());

            m_setLayouts.push_back(
                DescriptorSetLayout::create(m_context.device(), &createInfo, nullptr));
        }

        auto shaderModuleCreateInfo =
            ShaderModuleCreateInfo{}
                .codeSize(spvReflectGetCodeSize(module.get()))
                .pCode(spvReflectGetCode(module.get()));

        m_shaderModules.push_back(
            ShaderModule::create(m_context.device(), &shaderModuleCreateInfo, nullptr));

        m_shaderStageCreateInfos.push_back(PipelineShaderStageCreateInfo{}
                .stage(shaderStageFlag)
                .module(m_shaderModules.back())
                .pName("main"));
    }

    auto pipelineLayoutCreateInfo =
        PipelineLayoutCreateInfo{}
            .setLayoutCount(m_setLayouts.size())
            .pSetLayouts(m_setLayouts.data())
            //  TO DO: push constants impl
            .pushConstantRangeCount(0);

    m_pipelineLayout =
        PipelineLayout::create(m_context.device(), &pipelineLayoutCreateInfo, nullptr);
}

}    //  namespace renderer::vk
