#pragma once

#include <icompute_pipeline.hpp>

namespace ogl {

class GraphicsContext;

class ComputePipeline : public IComputePipeline
{
public:
    ComputePipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~ComputePipeline();

    virtual void bind(::OperationContext& context) override;
    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

    virtual ComputeDimensions computeDimensions() const override;

private:
    const GraphicsContext& m_context;
    const ComputeDimensions m_computeDimensions;
};

}    //  namespace ogl
