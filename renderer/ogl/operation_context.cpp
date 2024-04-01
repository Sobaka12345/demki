#include "operation_context.hpp"

#include "computer.hpp"
#include "ispecific_operation_target.hpp"
#include "renderer.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"

#include <operation_context.hpp>

namespace renderer::ogl {


OperationContext::OperationContext(Computer* computer)
    : computer(computer)
{}

OperationContext::OperationContext(Renderer* renderer)
    : renderer(renderer)
{}

OperationContext::OperationContext(OperationContext&& other)
    : graphicsPipeline(std::move(other.graphicsPipeline))
    , computePipeline(std::move(other.computePipeline))
    , renderer(std::move(other.renderer))
    , computer(std::move(other.computer))
{
    other.renderer = nullptr;
    other.computer = nullptr;
}

OperationContext::~OperationContext() {}

void OperationContext::submit(renderer::OperationContext& context)
{
    if (renderer) renderer->finish(context);
    if (computer) computer->finish(context);
}

void OperationContext::waitForOperation(OperationContext& other)
{
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glWaitSync(sync, 0, GL_TIMEOUT_IGNORED);
    glFlush();
}

void OperationContext::setScissors(Scissors scissors) const
{
    glScissor(scissors.x, scissors.y, scissors.width, scissors.height);
}

void OperationContext::setViewport(Viewport viewport) const
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glDepthRangef(viewport.minDepth, viewport.maxDepth);
}

IPipeline* OperationContext::pipeline()
{
    if (graphicsPipeline) return graphicsPipeline;
    if (computePipeline) return computePipeline;

    return nullptr;
}

IOperationTarget* OperationContext::operationTarget()
{
    if (mainTarget) return mainTarget;
    return specificTarget->toBase();
}

}    //  namespace renderer::ogl
