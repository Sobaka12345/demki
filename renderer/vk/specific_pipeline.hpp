#pragma once

#include "handles/descriptor_set.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/pipeline.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/shader_module.hpp"

#include "graphics_context.hpp"

#include "../utils.hpp"

#include <ipipeline.hpp>
#include <ishader_interface.hpp>

struct SpvReflectShaderModule;

namespace renderer::vk {

struct ISpecificPipelineDescriptor;

class ISpecificPipeline
{
public:
    virtual IPipeline* toBase() = 0;

protected:
    friend struct ISpecificPipelineDescriptor;

    ISpecificPipeline(GraphicsContext& context)
        : m_context(context)
    {}

    ~ISpecificPipeline();

    void initModules(std::span<const std::shared_ptr<SpvReflectShaderModule>> module);

protected:
    GraphicsContext& m_context;

    std::vector<PipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
    std::map<uint32_t, WriteDescriptorSet> m_writeDescriptorSetTemplateByBinding;
    std::map<uint32_t, size_t> m_descriptorSetLayoutIdByBinding;
    handles::ShaderModule::Vector<> m_shaderModules;
    handles::DescriptorSetLayout::Vector<> m_setLayouts;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorPool m_descriptorPool;
};

struct ISpecificPipelineDescriptor : public IPipeline::Descriptor
{
    ISpecificPipelineDescriptor(ISpecificPipeline& specificPipeline);

    virtual void bind(renderer::OperationContext& context) override;

    void initDynamicOffset(uint32_t bindingId);
    void setDynamicOffset(uint32_t bindingId, uint32_t offset);
    virtual void setBinding(uint32_t bindingId, std::shared_ptr<IShaderResource> resource) override;
    VkDescriptorSet descriptorSet(uint32_t bindingId) const;
    WriteDescriptorSet writeDescriptorSetTemplate(uint32_t bindingId) const;
    VkPipelineLayout pipelineLayout() const;

    std::vector<uint32_t> dynamicOffsets;
    handles::DescriptorSet::Vector<> descriptorSets;
    ISpecificPipeline& specificPipeline;
    std::map<uint32_t, size_t> bindingIdToDynamicOffsetId;
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
