#include "specific_shader_resource.hpp"

#include "operation_context.hpp"
#include "pipeline.hpp"

namespace renderer::vk {

std::vector<handles::WriteDescriptorSet> ISpecificShaderResource::descriptorSetWrites(
    renderer::vk::OperationContext& context,
    uint32_t bindingId,
    ShaderInterfaceHandle& handle) const
{
    DASSERT(context.pipelineBindContext, "no pipeline bind context attached");

    auto& pipelineBindContext = *context.pipelineBindContext;
    const auto& sets = pipelineBindContext.sets;
    const auto& setInfo = pipelineBindContext.info;

    handle.assureDescriptorCount(sets.size());

    std::vector<handles::WriteDescriptorSet> writes;
    writes.reserve(sets.size());
    const auto& binding = setInfo.binding(bindingId);
    for (size_t i = 0; i < sets.size(); ++i)
    {
        writes.emplace_back(
            handles::WriteDescriptorSet{}
                .dstSet(*sets[i])
                .dstBinding(binding.binding)
                .descriptorType(binding.descriptorType)
                .descriptorCount(binding.descriptorCount)
                .pBufferInfo(binding.descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ?
                        &handle.descriptor(i)->descriptorBufferInfo :
                        nullptr)
                .pImageInfo(binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ?
                        &handle.descriptor(i)->descriptorImageInfo :
                        nullptr));
    }

    return writes;
}

}    //  namespace renderer::vk
