#include "compute_pipeline.hpp"

#include <operation_context.hpp>

namespace ogl {

ComputePipeline::ComputePipeline(const GraphicsContext& context, CreateInfo createInfo)
    : Pipeline(context, createInfo)
    , m_computeDimensions(createInfo.computeDimensions())
{
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

ComputePipeline::~ComputePipeline() {}

void ComputePipeline::bind(::OperationContext& context)
{
    get(context).computePipeline = this;

    glUseProgram(m_shaderProgram);
}

IComputePipeline::ComputeDimensions ComputePipeline::computeDimensions() const
{
    return m_computeDimensions;
}


}    //  namespace ogl
