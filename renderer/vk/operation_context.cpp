#include "operation_context.hpp"

#include "handles/command_buffer.hpp"
#include "handles/render_pass.hpp"

#include "graphics_pipeline.hpp"
#include "renderer.hpp"
#include "computer.hpp"
#include "compute_pipeline.hpp"
#include "ispecific_operation_target.hpp"

#include <vulkan/vulkan_core.h>
#include <operation_context.hpp>
#include <ipipeline.hpp>

namespace vk {

OperationContext::OperationContext(Computer* computer)
    : computer(computer)
{}

OperationContext::OperationContext(Renderer* renderer)
    : renderer(renderer)
{}

OperationContext::OperationContext(OperationContext&& other)
    : graphicsPipeline(std::move(other.graphicsPipeline))
    , computePipeline(std::move(other.computePipeline))
    , waitSemaphores(std::move(other.waitSemaphores))
    , framebuffer(std::move(other.framebuffer))
    , commandBuffer(std::move(other.commandBuffer))
    , specificTarget(std::move(other.specificTarget))
    , renderer(std::move(other.renderer))
    , computer(std::move(other.computer))
    , renderPass(std::move(other.renderPass))
    , mainTarget(std::move(other.mainTarget))
{
    other.renderer = nullptr;
    other.computer = nullptr;
    other.computePipeline = nullptr;
    other.graphicsPipeline = nullptr;
}

OperationContext::~OperationContext() {}

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

void OperationContext::submit(::OperationContext& context)
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
    commandBuffer->setScissor(toVkScissors(scissors));
}

void OperationContext::setViewport(Viewport viewport) const
{
    commandBuffer->setViewport(toVkViewport(viewport));
}

}    //  namespace vk
