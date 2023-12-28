#include "compute_pipeline.hpp"

namespace ogl {

ComputePipeline::ComputePipeline(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_computeDimensions(std::move(createInfo.computeDimensions()))
{}

ComputePipeline::~ComputePipeline() {}

void ComputePipeline::bind(OperationContext& context) {}

FragileSharedPtr<IPipelineBindContext> ComputePipeline::bindContext(
    const IShaderInterfaceContainer& container)
{}

IComputePipeline::ComputeDimensions ComputePipeline::computeDimensions() const {}


}    //  namespace ogl
