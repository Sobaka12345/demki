#include "operation_context.hpp"

#include "handles/command_buffer.hpp"
#include "handles/render_pass.hpp"

#include "graphics_pipeline.hpp"
#include "renderer.hpp"
#include "computer.hpp"
#include "compute_pipeline.hpp"
#include "specific_operation_target.hpp"

#include <vulkan/vulkan_core.h>
#include <operation_context.hpp>
#include <ipipeline.hpp>

namespace renderer::vk {

OperationContext::OperationContext(Computer* computer)
    : computer(computer)
{}

OperationContext::OperationContext(Renderer* renderer)
    : renderer(renderer)
{}

OperationContext::~OperationContext() {}

IPipeline* OperationContext::pipeline()
{
    return specificPipeline()->toBase();
}

ISpecificPipeline* OperationContext::specificPipeline()
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

void OperationContext::draw(uint32_t count, uint32_t instances)
{
    vkCmdDraw(commandBuffer, count, instances, 0, 0);
}

void OperationContext::submit(renderer::OperationContext& context)
{
    if (renderer) renderer->finish(context);
    if (computer) computer->finish(context);
}

inline VkViewport toVkViewport(const Viewport& val)
{
    return VkViewport{
        .x = val.x,
        .y = val.height,
        .width = val.width,
        .height = -val.height,
        .minDepth = val.minDepth,
        .maxDepth = val.maxDepth,
    };
}

inline VkRect2D toVkScissors(const Scissors& scissors)
{
    return VkRect2D{
        .offset =
            VkOffset2D{
                .x = scissors.x,
                .y = scissors.y,
            },
        .extent =
            VkExtent2D{
                .width = scissors.width,
                .height = scissors.height,
            },
    };
}

void OperationContext::waitForOperation(OperationContext& other)
{
    other.specificTarget->populateWaitInfo(*this);
    specificTarget->waitFor(*this);
}

void OperationContext::setScissors(Scissors scissors) const
{
    const auto s = toVkScissors(scissors);
    vkCmdSetScissor(commandBuffer, 0, 1, &s);
}

void OperationContext::setViewport(Viewport viewport) const
{
    const auto v = toVkViewport(viewport);
    vkCmdSetViewport(commandBuffer, 0, 1, &v);
}

}    //  namespace renderer::vk
