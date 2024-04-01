#pragma once

#include "pipeline.hpp"

#include <icompute_pipeline.hpp>

namespace renderer::ogl {

class GraphicsContext;

class ComputePipeline
    : public Pipeline
    , public IComputePipeline
{
public:
    ComputePipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~ComputePipeline();

    virtual void bind(renderer::OperationContext& context) override;

    virtual ComputeDimensions computeDimensions() const override;

private:
    const ComputeDimensions m_computeDimensions;
};

}    //  namespace renderer::ogl
