#include "pipeline.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"

#include "descriptor_set_provider.hpp"
#include "graphics_context.hpp"
#include "specific_operation_target.hpp"

#include <ishader_interface_container.hpp>

namespace renderer::vk {

PipelineBindContext::PipelineBindContext(CreateInfo createInfo)
    : info(std::move(createInfo))
    , currentSetIndex(0)
{
    dynamicOffsets.reserve(info.container.layout().size());
}

void PipelineBindContext::bind(renderer::OperationContext& context)
{
    currentSetIndex = sets.empty() ? 0 : (currentSetIndex + 1) % sets.size();

    dynamicOffsets.clear();
    auto& specificContext = get(context);
    specificContext.pipelineBindContext = this;
    const uint32_t descriptorsRequired = specificContext.specificTarget->descriptorsRequired();

    uint32_t bindingId = 0;
    if (descriptorsRequired > sets.size()) [[unlikely]]
    {
        while (descriptorsRequired > sets.size())
        {
            sets.emplace_back(info.descriptorSetProvider.set(info.descriptorSetLayout));
        }

        for (auto& descriptor : info.container)
        {
            descriptor->adapt(context, bindingId);
            descriptor->bind(context, bindingId++);
        }
    }
    else [[likely]]
    {
        for (const auto& descriptor : info.container)
        {
            descriptor->bind(context, bindingId++);
        }
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

std::shared_ptr<IPipelineBindContext> Pipeline::bindContext(IShaderInterfaceContainer& container)
{
    const auto containerId = container.id();
    auto& [setId, layout] = m_setLayouts.at(containerId);

    return std::shared_ptr<IPipelineBindContext>{ newBindContext({
        .setId = setId,
        .container = container,
        .bindingIndices = m_bindingIndices.at(containerId),
        .descriptorSetProvider = m_descriptorSetProviders.at(containerId),
        .descriptorSetLayout = layout,
    }) };
}

}    //  namespace renderer::vk
