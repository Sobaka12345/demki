#pragma once

#include "pipeline.hpp"

#include "handles/compute_pipeline.hpp"

#include <icompute_pipeline.hpp>

namespace renderer::vk {

class ComputePipeline
    : public Pipeline
    , public IComputePipeline
{
    struct BindContext : public Pipeline::BindContext
    {
        using Pipeline::BindContext::BindContext;

        virtual void bind(renderer::OperationContext& context,
            const IShaderInterfaceContainer& container) override;
    };

private:
    static ComputePipelineCreateInfo defaultPipeline();

public:
    ComputePipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~ComputePipeline();

    virtual ComputeDimensions computeDimensions() const override;

    virtual void bind(renderer::OperationContext& context) override;

private:
    const handles::Pipeline& pipeline(const vk::OperationContext& context);

    virtual ComputePipeline::BindContext* newBindContext(
        BindContext::DescriptorSetInfo descriptorSetInfo) const override;

private:
    std::vector<ShaderInfo> m_shaders;
    ComputeDimensions m_computeDimensions;
    std::map<const handles::RenderPass*, handles::ComputePipeline> m_pipelines;
};

}    //  namespace renderer::vk
