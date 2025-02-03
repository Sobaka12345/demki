#pragma once

#include "graphics_context.hpp"

#include "../utils.hpp"

#include <ipipeline.hpp>
#include <ishader_interface.hpp>

namespace renderer::vk {

class ISpecificPipeline
{
public:
    virtual IPipeline* toBase() = 0;

protected:
    ISpecificPipeline(GraphicsContext& context)
        : m_context(context)
    {}

protected:
    const GraphicsContext& m_context;
};

template <typename IBase>
using SpecificPipeline = SpecificBase<IBase, ISpecificPipeline>;

[[nodiscard]] inline constexpr VkShaderStageFlagBits toShaderStageFlags(ShaderStage type)
{
    switch (type)
    {
        case ShaderStage::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case ShaderStage::GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::TASK: return VK_SHADER_STAGE_TASK_BIT_EXT;
        case ShaderStage::MESH: return VK_SHADER_STAGE_MESH_BIT_EXT;
        default: ASSERT(false, "unknown shader stage");
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

[[nodiscard]] inline constexpr VkDescriptorType toDescriptorType(ShaderBlockType type)
{
    switch (type)
    {
        case ShaderBlockType::UNIFORM_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case ShaderBlockType::UNIFORM_STATIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case ShaderBlockType::SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case ShaderBlockType::STORAGE: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default: ASSERT(false, "unknown descriptor type");
    };

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

}    //  namespace renderer::vk
