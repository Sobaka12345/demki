#include "operation_context.hpp"

#include "computer.hpp"
#include "renderer.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"

#include <operation_context.hpp>

namespace ogl {


OperationContext::OperationContext(Computer* computer)
    : computer(computer)
{}

OperationContext::OperationContext(Renderer* renderer)
    : renderer(renderer)
{}

OperationContext::OperationContext(OperationContext&& other)
    : graphicsPipeline(std::move(other.graphicsPipeline))
    , computePipeline(std::move(other.computePipeline))
    , computeTarget(std::move(other.computeTarget))
    , renderTarget(std::move(other.renderTarget))
    , renderer(std::move(other.renderer))
    , computer(std::move(other.computer))
{
    other.renderer = nullptr;
    other.computer = nullptr;
}

OperationContext::~OperationContext() {}

void OperationContext::submit(::OperationContext& context)
{
    if (renderer) renderer->finish(context);
    if (computer) computer->finish(context);
}

void OperationContext::waitForOperation(OperationContext& other) {}

void OperationContext::setScissors(Scissors scissors) const {}

void OperationContext::setViewport(Viewport viewport) const {}

IPipeline* OperationContext::pipeline()
{
    if (graphicsPipeline) return graphicsPipeline;
    if (computePipeline) return computePipeline;

    return nullptr;
}

}    //  namespace ogl
