#include "graphics_pipeline.hpp"

#include <operation_context.hpp>
#include <utils.hpp>

namespace renderer::ogl {

float toGLSampleShadingCoefficient(IGraphicsPipeline::CreateInfo::SampleShading sampleShading)
{
    switch (sampleShading)
    {
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_0_PERCENT: return 0.0f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_20_PERCENT: return 0.2f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_40_PERCENT: return 0.4f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_60_PERCENT: return 0.6f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_80_PERCENT: return 0.8f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_100_PERCENT: return 1.0f;
    }

    ASSERT(false, "sample shading not declared");
    return 0.0f;
}

int toGLFrontFace(IGraphicsPipeline::CreateInfo::FrontFace frontFace)
{
    switch (frontFace)
    {
        case IGraphicsPipeline::CreateInfo::CLOCKWISE: return GL_CW;
        case IGraphicsPipeline::CreateInfo::COUNTER_CLOCKWISE: return GL_CCW;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLCullMode(IGraphicsPipeline::CreateInfo::CullMode cullMode)
{
    switch (cullMode)
    {
        case IGraphicsPipeline::CreateInfo::BACK: return GL_BACK;
        case IGraphicsPipeline::CreateInfo::FRONT: return GL_FRONT;
        case IGraphicsPipeline::CreateInfo::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLPolygonMode(IGraphicsPipeline::CreateInfo::PolygonMode polygonMode)
{
    switch (polygonMode)
    {
        case IGraphicsPipeline::CreateInfo::POINT: return GL_POINT;
        case IGraphicsPipeline::CreateInfo::LINE: return GL_LINE;
        case IGraphicsPipeline::CreateInfo::FILL: return GL_FILL;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLTopology(IGraphicsPipeline::CreateInfo::PrimitiveTopology topology)
{
    switch (topology)
    {
        case IGraphicsPipeline::CreateInfo::LINES: return GL_LINES;
        case IGraphicsPipeline::CreateInfo::TRIANGLES: return GL_TRIANGLES;
        case IGraphicsPipeline::CreateInfo::POINTS: return GL_POINTS;
    }

    ASSERT(false, "not implemented");
    return -1;
}

GraphicsPipeline::GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo)
    : Pipeline(context, createInfo)
    , m_primitiveTopology(toGLTopology(createInfo.primitiveTopology()))
    , m_cullMode(toGLCullMode(createInfo.cullMode()))
    , m_polygonMode(toGLPolygonMode(createInfo.polygonMode()))
    , m_frontFace(toGLFrontFace(createInfo.frontFace()))
    , m_sampleShading(toGLSampleShadingCoefficient(createInfo.sampleShading()))
{}

GraphicsPipeline::~GraphicsPipeline() {}

int GraphicsPipeline::primitiveTopology() const
{
    return m_primitiveTopology;
}

void GraphicsPipeline::bind(renderer::OperationContext& context)
{
    get(context).graphicsPipeline = this;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (m_sampleShading > 0.01f)
    {
        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(m_sampleShading);
    }
    else
    {
        glDisable(GL_SAMPLE_SHADING);
    }

    glEnable(GL_CULL_FACE);
    glCullFace(m_cullMode);
    glFrontFace(m_frontFace);
    glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

    glUseProgram(m_shaderProgram);
}

}    //  namespace renderer::ogl
