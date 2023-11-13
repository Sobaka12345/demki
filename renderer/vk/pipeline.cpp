#include "pipeline.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

#include <boost/pfr.hpp>

#include <algorithm>

namespace vk {

template <typename T>
VkFormat attrubuteFormat(const T& attribute)
{
    if constexpr (std::is_same_v<T, float>)
    {
        return VK_FORMAT_R32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec2>)
    {
        return VK_FORMAT_R32G32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec3>)
    {
        return VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec4>)
    {
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    return VK_FORMAT_UNDEFINED;
}

void Pipeline::BindContext::bind(::RenderContext& context,
    const IShaderInterfaceContainer& container)
{
    auto& specContext = get(context);
    const auto uniforms = container.dynamicUniforms();
    std::vector<uint32_t> offsets(uniforms.size(), 0);
    std::for_each(uniforms.begin(), uniforms.end(), [](auto& descriptor) {
        DASSERT(!descriptor.handle.expired(), "handle is expired or has not been initialized");
        struct ResourceIdVisitor : public ShaderInterfaceHandleVisitor
        {
            void visit(ShaderInterfaceHandle& handle) override { handle.assureDescriptorCount(2); }
        } visitor;
        descriptor.handle.lock()->accept(visitor);
    });
    //    std::vector<uint32_t> offsets(uniforms.size());
    //    std::transform(uniforms.begin(), uniforms.end(), offsets.begin(), [](const auto&
    //    descriptor) {
    //        DASSERT(!descriptor.handle.expired(), "handle is expired or has not been
    //        initialized"); return descriptor.handle.lock()->resourceOffset();
    //    });

    //  TO DO: RETURN DYNAMIC OFFSETS

    specContext.commandBuffer->bindDescriptorSet(specContext.pipeline->layout(), setId, *set,
        offsets);
}

Pipeline::Pipeline(const GraphicsContext& _context, IPipeline::CreateInfo createInfo)
    : m_context(_context)
    , m_createInfo(std::move(createInfo))
{
    m_bindingDescriptions.resize(m_createInfo.inputs().size());
    m_attributeDescriptions.reserve(m_createInfo.inputs().size() * 3);

    for (uint32_t i = 0; i < m_createInfo.inputs().size(); ++i)
    {
        std::visit(
            [&](auto& val) {
                m_bindingDescriptions[i].binding = i;
                m_bindingDescriptions[i].stride = sizeof(decltype(val));
                m_bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                uint32_t j = 0;
                boost::pfr::for_each_field(val, [&](auto& subVal) {
                    m_attributeDescriptions.push_back({
                        .location = j++,
                        .binding = i,
                        .format = attrubuteFormat(subVal),
                        .offset = static_cast<uint32_t>(
                            reinterpret_cast<int8_t*>(&subVal) - reinterpret_cast<int8_t*>(&val)),
                    });
                });
            },
            m_createInfo.inputs()[i]);
    }

    //  TO DO: remove this cringe
    constexpr int poolMultiplier = 10000;

    std::vector<VkDescriptorSetLayout> layouts;
    std::vector<handles::DescriptorPoolSize> poolSizes;
    for (auto& set : m_createInfo.uniformContainers())
    {
        std::vector<handles::DescriptorSetLayoutBinding> setLayoutBindings;
        auto span = set.second;
        for (auto& uniform : span)
        {
            const auto type = toDescriptorType(uniform.type);
            setLayoutBindings.push_back(
                handles::DescriptorSetLayoutBinding{}
                    .descriptorCount(uniform.count)
                    .binding(static_cast<uint32_t>(uniform.id))
                    .descriptorType(type)
                    .stageFlags(toShaderStageFlags(uniform.stage)));

            if (auto iter = std::find_if(poolSizes.begin(), poolSizes.end(),
                    [&](auto& x) { return x.type() == type; });
                iter != poolSizes.end())
            {
				iter->descriptorCount((iter->descriptorCount() + 1) * poolMultiplier);
			}
			else
			{
                poolSizes.push_back(handles::DescriptorPoolSize{}.type(type).descriptorCount(
					uniform.count * poolMultiplier));
            }
        }

        const auto& [iter, _] = m_setLayouts.emplace(set.first,
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

    m_pool = std::make_unique<handles::DescriptorPool>(m_context.device(),
        handles::DescriptorPoolCreateInfo{}
			.maxSets(poolMultiplier)
            .poolSizeCount(poolSizes.size())
            .pPoolSizes(poolSizes.data())
            .flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT));
}

Pipeline::~Pipeline()
{
	m_bindContexts.clear();
	m_pool.reset();
}

void Pipeline::bind(::RenderContext& context)
{
    auto& specContext = get(context);
    specContext.commandBuffer->bindPipeline(pipeline(specContext));
    specContext.pipeline = this;
}

std::weak_ptr<IPipeline::IBindContext> Pipeline::bindContext(
    const IShaderInterfaceContainer& container)
{
    struct ResourceIdVisitor : public ShaderInterfaceHandleVisitor
    {
        void visit(ShaderInterfaceHandle& handle) override
        {
            result = &handle.currentDescriptor()->id;
        }

        ShaderResource::Descriptor::Id* result = nullptr;
    } resourceIdVisitor;

    std::span descriptors = container.uniforms();
    std::set<ShaderResource::Descriptor::Id> keySet;
    std::transform(descriptors.begin(), descriptors.end(), std::inserter(keySet, keySet.begin()),
        [&resourceIdVisitor](const auto& x) {
            DASSERT(!x.handle.expired(), "descriptor handle is expired or was not initialized");
            x.handle.lock()->accept(resourceIdVisitor);
            return *resourceIdVisitor.result;
        });

    if (auto iter = m_bindContexts.find(keySet); iter != m_bindContexts.end())
    {
        return iter->second;
    }

    struct BufferInfoVisitor : public ShaderInterfaceHandleVisitor
    {
        void visit(ShaderInterfaceHandle& handle) override
        {
            result = &handle.currentDescriptor()->descriptorBufferInfo;
        }

        DescriptorBufferInfo* result = nullptr;
    } bufferInfoVisitor;

    struct ImageInfoVisitor : public ShaderInterfaceHandleVisitor
    {
        void visit(ShaderInterfaceHandle& handle) override
        {
            result = &handle.currentDescriptor()->descriptorImageInfo;
        }

        DescriptorImageInfo* result = nullptr;
    } imageInfoVisitor;

    auto& [layoutId, layout] = m_setLayouts.at(container.id());

    std::vector<handles::DescriptorSet::Write> writes;
    for (const auto& descriptor : descriptors)
    {
        if (descriptor.binding.type == ShaderBlockType::SAMPLER)
        {
            descriptor.handle.lock()->accept(imageInfoVisitor);
            writes.push_back(handles::DescriptorSet::Write{
                .imageInfo = *imageInfoVisitor.result,
                .layoutBinding = layout.binding(static_cast<int32_t>(descriptor.binding.id)),
            });
        }
        else
        {
            descriptor.handle.lock()->accept(bufferInfoVisitor);
            writes.push_back(handles::DescriptorSet::Write{
                .bufferInfo = *bufferInfoVisitor.result,
                .layoutBinding = layout.binding(static_cast<int32_t>(descriptor.binding.id)),
            });
        }
    }

    auto result = std::shared_ptr<BindContext>(new BindContext);
    result->set = m_pool->allocateSet(layout);
	result->set->write(writes);
    result->setId = layoutId;
	m_bindContexts.emplace(keySet, result);

    return result;
}

}    //  namespace vk
