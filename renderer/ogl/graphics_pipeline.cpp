#include "graphics_pipeline.hpp"

namespace ogl {

GraphicsPipeline::GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

GraphicsPipeline::~GraphicsPipeline() {}

void GraphicsPipeline::bind(OperationContext& context) {}

FragileSharedPtr<IPipelineBindContext> GraphicsPipeline::bindContext(
    const IShaderInterfaceContainer& container)
{}


}    //  namespace ogl
