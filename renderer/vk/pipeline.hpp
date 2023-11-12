#pragma once

#include "uniform.hpp"
#include "uniform_handle.hpp"

#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/graphics_pipeline.hpp"

#include <ipipeline.hpp>

#include <map>
#include <set>

namespace vk {

class Renderer;
class RenderContext;
class GraphicsContext;

class UniformHandle;

namespace handles {
class DescriptorPool;
class PipelineLayout;
class RenderPass;
}

class Pipeline : public IPipeline
{
    struct SetHasher
    {
        size_t operator()(const std::set<UBODescriptor::Id>& e) const
        {
            size_t result = 0;
            uint64_t pow = 1;
            for (UBODescriptor::Id el : e)
            {
                result += el.resourceId * pow + el.descriptorId * pow * 2 + el.bufferId * pow * 3;
                pow *= 10;
            }

            return result;
        }
    };

    struct BindContext : public IPipeline::IBindContext
    {
        virtual void bind(::RenderContext& context, const IUniformContainer& container) override;
        std::shared_ptr<handles::DescriptorSet> set;
        uint32_t setId;
    };

private:
    static GraphicsPipelineCreateInfo defaultPipeline();

public:
    Pipeline(const GraphicsContext& context, IPipeline::CreateInfo createInfo);
    ~Pipeline();

    virtual void bind(::RenderContext& context) override;
    virtual std::weak_ptr<IBindContext> bindContext(const IUniformContainer& container) override;

    const handles::PipelineLayout& layout() const { return *m_pipelineLayout; }

private:
    const handles::GraphicsPipeline& pipeline(const vk::RenderContext& context);

private:
    const GraphicsContext& m_context;

    std::unique_ptr<handles::DescriptorPool> m_pool;
    std::unordered_map<std::set<UBODescriptor::Id>, std::shared_ptr<BindContext>, SetHasher>
        m_bindContexts;
    std::unordered_map<uint32_t, std::pair<uint32_t, handles::DescriptorSetLayout>> m_setLayouts;

    std::unique_ptr<handles::PipelineLayout> m_pipelineLayout;

    std::unordered_map<UniformID, uint32_t> m_descriptorsCount;
    std::map<const handles::RenderPass*, handles::GraphicsPipeline> m_pipelines;

    IPipeline::CreateInfo m_createInfo;

    std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
};

}    //  namespace vk
