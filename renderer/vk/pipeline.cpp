#include "pipeline.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"

#include "descriptor_set_provider.hpp"
#include "graphics_context.hpp"
#include "ispecific_operation_target.hpp"

#include <algorithm>

namespace vk {

ShaderInterfaceHandle::TypeVisitor Pipeline::s_handleVisitor;

Pipeline::BindContext::BindContext(DescriptorSetInfo descriptorSetInfo)
    : descriptorSetInfo(descriptorSetInfo)
{}

void Pipeline::BindContext::bind(::OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    const uint32_t descriptorsRequired = get(context).specificTarget->descriptorsRequired();

    std::span descriptors = container.uniforms();
    std::vector<ShaderResource::Descriptor::Id> keyVector;
    keyVector.reserve(descriptors.size());
    for (const auto& descriptor : descriptors)
    {
        DASSERT(!descriptor.handle.expired(),
            "descriptor handle is expired or was not initialized");
        descriptor.handle.lock()->accept(s_handleVisitor);

        s_handleVisitor->assureDescriptorCount(descriptorsRequired);

        const auto id = s_handleVisitor->currentDescriptor()->id;

        if (descriptor.binding.type == ShaderBlockType::UNIFORM_DYNAMIC)
        {
            keyVector.push_back({
                //  dynamic descriptors use dynamic offsets and can
                //  share same descriptor set given the same buffer ID
                .descriptorId = 0,
                .bufferId = id.bufferId,
                .resourceId = id.resourceId,
            });
        }
        else
        {
            keyVector.push_back(id);
        }
    }

    if (auto iter = sets.find(keyVector); iter != sets.end())
    {
        currentSet = iter->second;
    }
    else
    {
        std::vector<handles::DescriptorSet::Write> writes;
        for (uint32_t i = 0; i < descriptors.size(); ++i)
        {
            descriptors[i].handle.lock()->accept(s_handleVisitor);
            if (descriptors[i].binding.type == ShaderBlockType::SAMPLER)
            {
                writes.push_back(handles::DescriptorSet::Write{
                    .imageInfo = s_handleVisitor->currentDescriptor()->descriptorImageInfo,
                    .layoutBinding = descriptorSetInfo.descriptorSetLayout.binding(
                        descriptorSetInfo.bindingIndices[i]),
                });
            }
            else
            {
                writes.push_back(handles::DescriptorSet::Write{
                    .bufferInfo = s_handleVisitor->currentDescriptor()->descriptorBufferInfo,
                    .layoutBinding = descriptorSetInfo.descriptorSetLayout.binding(
                        descriptorSetInfo.bindingIndices[i]),
                });
            }
        }

        auto [newEl, _] = sets.emplace(keyVector,
            descriptorSetInfo.descriptorSetProvider.set(descriptorSetInfo.descriptorSetLayout));
        newEl->second->write(writes);
        currentSet = newEl->second;
    }
}

void Pipeline::init(const std::vector<InterfaceContainerInfo>& interfaceContainers)
{
    std::vector<VkDescriptorSetLayout> layouts;
    uint32_t bindingId = 0;
    for (auto& containerInfo : interfaceContainers)
    {
        std::vector<handles::DescriptorPoolSize> poolSizes;
        std::vector<handles::DescriptorSetLayoutBinding> setLayoutBindings;
        for (auto& uniform : containerInfo.layout)
        {
            const auto type = toDescriptorType(uniform.type);
            setLayoutBindings.push_back(
                handles::DescriptorSetLayoutBinding{}
                    .descriptorCount(uniform.count)
                    .binding(bindingId)
                    .descriptorType(type)
                    .stageFlags(toShaderStageFlags(uniform.stage)));

            m_bindingIndices[containerInfo.id].push_back(bindingId++);

            poolSizes.push_back(
                handles::DescriptorPoolSize{}.type(type).descriptorCount(containerInfo.batchSize));
        }

        m_descriptorSetProviders.emplace(std::piecewise_construct,
            std::forward_as_tuple(containerInfo.id),
            std::forward_as_tuple(m_context,
                handles::DescriptorPoolCreateInfo{}
                    .maxSets(containerInfo.batchSize)
                    .poolSizeCount(poolSizes.size())
                    .pPoolSizes(poolSizes.data())
                    .flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)));

        const auto& [iter, _] = m_setLayouts.emplace(containerInfo.id,
            std::pair{ layouts.size(),
                handles::DescriptorSetLayout(m_context.device(),
                    handles::DescriptorSetLayoutCreateInfo{}
                        .bindingCount(setLayoutBindings.size())
                        .pBindings(setLayoutBindings.data())) });
        layouts.push_back(iter->second.second);
    }

    m_pipelineLayout = std::make_unique<handles::PipelineLayout>(m_context.device(),
        handles::PipelineLayoutCreateInfo{}
            .setLayoutCount(layouts.size())
            .pSetLayouts(layouts.data())
            //  TO DO: push constants impl
            .pushConstantRangeCount(0));
}

Pipeline::~Pipeline()
{
    m_bindContexts.clear();
    m_descriptorSetProviders.clear();
}

FragileSharedPtr<IPipelineBindContext> Pipeline::bindContext(
    const IShaderInterfaceContainer& container)
{
    const auto containerId = container.id();
    const auto containerTypeId = container.typeId();

    if (auto iter = m_bindContexts.find(containerTypeId); iter != m_bindContexts.end())
    {
        return iter->second;
    }

    auto& [setId, layout] = m_setLayouts.at(containerId);

    auto [contextIter, _] = m_bindContexts.emplace(containerTypeId, 
        newBindContext({
            .setId = setId,
            .bindingIndices = m_bindingIndices.at(containerId),
            .descriptorSetProvider = m_descriptorSetProviders.at(containerId),
            .descriptorSetLayout = layout,
    }));

    contextIter->second.setFragile(true);

    return contextIter->second;
}

}    //  namespace vk
