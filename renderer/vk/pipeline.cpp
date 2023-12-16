#include "pipeline.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

#include "descriptor_set_provider.hpp"
#include "graphics_context.hpp"
#include "renderer.hpp"
#include "computer.hpp"
#include "ioperation_target.hpp"

#include <algorithm>

namespace vk {

struct ResourceIdVisitor : public ShaderInterfaceHandleVisitor
{
    void visit(ShaderInterfaceHandle& handle) override { result = &handle.currentDescriptor()->id; }

    ShaderResource::Descriptor::Id* result = nullptr;
};

struct BufferInfoVisitor : public ShaderInterfaceHandleVisitor
{
    void visit(ShaderInterfaceHandle& handle) override
    {
        result = &handle.currentDescriptor()->descriptorBufferInfo;
    }

    DescriptorBufferInfo* result = nullptr;
};

struct ImageInfoVisitor : public ShaderInterfaceHandleVisitor
{
    void visit(ShaderInterfaceHandle& handle) override
    {
        result = &handle.currentDescriptor()->descriptorImageInfo;
    }

    DescriptorImageInfo* result = nullptr;
};

struct AssureDescriptorsVisitor : public ShaderInterfaceHandleVisitor
{
    AssureDescriptorsVisitor(uint32_t count)
        : count(count){};

    void visit(ShaderInterfaceHandle& handle) override { handle.assureDescriptorCount(count); }

    uint32_t count = 1;
};

Pipeline::BindContext::BindContext(DescriptorSetInfo descriptorSetInfo)
    : descriptorSetInfo(descriptorSetInfo)
{}

void Pipeline::BindContext::bind(::OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    static BufferInfoVisitor bufferInfoVisitor;
    static ImageInfoVisitor imageInfoVisitor;
    static ResourceIdVisitor resourceIdVisitor;

    AssureDescriptorsVisitor assureDescriptorsVisitor(
        get(context).operationTarget()->descriptorsRequired());

    std::span descriptors = container.uniforms();
    std::vector<ShaderResource::Descriptor::Id> keyVector;
    keyVector.reserve(descriptors.size());
    std::transform(descriptors.begin(), descriptors.end(), std::back_inserter(keyVector),
        [&assureDescriptorsVisitor](const auto& x) {
            DASSERT(!x.handle.expired(), "descriptor handle is expired or was not initialized");
            x.handle.lock()->accept(resourceIdVisitor);
            x.handle.lock()->accept(assureDescriptorsVisitor);
            return *resourceIdVisitor.result;
        });

    if (auto iter = sets.find(keyVector); iter != sets.end())
    {
        currentSet = iter->second;
    }
    else
    {
        std::vector<handles::DescriptorSet::Write> writes;
        for (uint32_t i = 0; i < descriptors.size(); ++i)
        {
            if (descriptors[i].binding.type == ShaderBlockType::SAMPLER)
            {
                descriptors[i].handle.lock()->accept(imageInfoVisitor);
                writes.push_back(handles::DescriptorSet::Write{
                    .imageInfo = *imageInfoVisitor.result,
                    .layoutBinding = descriptorSetInfo.descriptorSetLayout.binding(
                        descriptorSetInfo.bindingIndices[i]),
                });
            }
            else
            {
                descriptors[i].handle.lock()->accept(bufferInfoVisitor);
                writes.push_back(handles::DescriptorSet::Write{
                    .bufferInfo = *bufferInfoVisitor.result,
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
	auto& [setId, layout] = m_setLayouts.at(container.id());

    Pipeline::BindContext* context = newBindContext({
		.setId = setId,
		.bindingIndices = m_bindingIndices.at(container.id()),
		.descriptorSetProvider = m_descriptorSetProviders.at(container.id()),
		.descriptorSetLayout = layout,
	});

    return m_bindContexts.emplace_back(context);
}

}    //  namespace vk
