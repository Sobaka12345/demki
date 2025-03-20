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
        // case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
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

ISpecificPipelineDescriptor::ISpecificPipelineDescriptor(ISpecificPipeline& specificPipeline)
    : specificPipeline(specificPipeline)
{
    descriptorSets.resize(specificPipeline.m_setLayouts.size());

    handles::DescriptorSet::allocate(specificPipeline.m_context.device(),
        DescriptorSetAllocateInfo{}
            .descriptorPool(specificPipeline.m_descriptorPool)
            .descriptorSetCount(specificPipeline.m_setLayouts.size())
            .pSetLayouts(specificPipeline.m_setLayouts.data()),
        descriptorSets.data());
}

void ISpecificPipelineDescriptor::bind(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    specContext.pipelineDescriptor = this;

    IPipeline::Descriptor::bind(context);
}

void ISpecificPipelineDescriptor::initDynamicOffset(uint32_t bindingId)
{
    bindingIdToDynamicOffsetId[bindingId] = dynamicOffsets.size();
    dynamicOffsets.push_back(0);
}

void ISpecificPipelineDescriptor::setDynamicOffset(uint32_t bindingId, uint32_t offset)
{
    dynamicOffsets[bindingIdToDynamicOffsetId[bindingId]] = offset;
}

void ISpecificPipelineDescriptor::setBinding(uint32_t bindingId,
    std::shared_ptr<IShaderResource> resource)
{
    IPipeline::Descriptor::setBinding(bindingId, resource);

    vk::OperationContext specContext;
    specContext.pipelineDescriptor = this;
    renderer::OperationContext context(std::move(specContext));
    resource->init(context, bindingId);
}

VkDescriptorSet ISpecificPipelineDescriptor::descriptorSet(uint32_t bindingId) const
{
    return descriptorSets[specificPipeline.m_descriptorSetLayoutIdByBinding[bindingId]];
}

WriteDescriptorSet ISpecificPipelineDescriptor::writeDescriptorSetTemplate(uint32_t bindingId) const
{
    return specificPipeline.m_writeDescriptorSetTemplateByBinding[bindingId].dstSet(
        descriptorSet(bindingId));
}

VkPipelineLayout ISpecificPipelineDescriptor::pipelineLayout() const
{
    return specificPipeline.m_pipelineLayout;
}

ISpecificPipeline::~ISpecificPipeline()
{
    DescriptorPool::destroy(m_context.device(), m_descriptorPool);
    PipelineLayout::destroy(m_context.device(), m_pipelineLayout);
    m_setLayouts.destroyAll(m_context.device());
    m_shaderModules.destroyAll(m_context.device());
}

void ISpecificPipeline::initModules(
    std::span<const std::shared_ptr<SpvReflectShaderModule>> modules)
{
    m_shaderStageCreateInfos.reserve(modules.size());

    //  TO DO: CHANGE
    constexpr uint32_t maxSets = 100;
    std::vector<DescriptorPoolSize> poolSizes;

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
                const VkDescriptorType descriptorType =
                    toDescriptorType(spirvBinding->descriptor_type);

                bindings[j]
                    .binding(spirvBinding->binding)
                    .descriptorCount(spirvBinding->count)
                    .descriptorType(descriptorType)
                    .stageFlags(shaderStageFlag);

                poolSizes.push_back(DescriptorPoolSize{}
                        .descriptorCount(maxSets * spirvBinding->count)
                        .type(descriptorType));

                m_writeDescriptorSetTemplateByBinding[spirvBinding->binding] =
                    WriteDescriptorSet{}
                        .descriptorCount(spirvBinding->count)
                        .descriptorType(descriptorType)
                        .dstBinding(spirvBinding->binding);

                m_descriptorSetLayoutIdByBinding[spirvBinding->binding] = m_setLayouts.size();
            }

            m_setLayouts.push_back(DescriptorSetLayout::create(m_context.device(),
                DescriptorSetLayoutCreateInfo{}
                    .bindingCount(bindings.size())
                    .pBindings(bindings.data())));
        }

        m_shaderModules.push_back(ShaderModule::create(m_context.device(),
            ShaderModuleCreateInfo{}
                .codeSize(spvReflectGetCodeSize(module.get()))
                .pCode(spvReflectGetCode(module.get()))));

        m_shaderStageCreateInfos.push_back(PipelineShaderStageCreateInfo{}
                .stage(shaderStageFlag)
                .module(m_shaderModules.back())
                .pName("main"));
    }

    m_pipelineLayout = PipelineLayout::create(m_context.device(),
        PipelineLayoutCreateInfo{}
            .setLayoutCount(m_setLayouts.size())
            .pSetLayouts(m_setLayouts.data())
            //  TO DO: push constants impl
            .pushConstantRangeCount(0));

    m_descriptorPool = DescriptorPool::create(m_context.device(),
        DescriptorPoolCreateInfo{}
            .maxSets(maxSets)
            .poolSizeCount(poolSizes.size())
            .pPoolSizes(poolSizes.data()));
}

}    //  namespace renderer::vk
