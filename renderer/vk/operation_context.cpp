#include "operation_context.hpp"

#include "handles/command_buffer.hpp"
#include "handles/render_pass.hpp"

#include "graphics_pipeline.hpp"
#include "renderer.hpp"
#include "computer.hpp"
#include "compute_pipeline.hpp"

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
    , finishSemaphores(std::move(other.finishSemaphores))
    , framebuffer(std::move(other.framebuffer))
    , commandBuffer(std::move(other.commandBuffer))
    , computeTarget(std::move(other.computeTarget))
    , renderTarget(std::move(other.renderTarget))
    , renderer(std::move(other.renderer))
    , computer(std::move(other.computer))
    , renderPass(std::move(other.renderPass))
{
    other.renderer = nullptr;
    other.computer = nullptr;
}

OperationContext::~OperationContext() {}

IPipeline* OperationContext::pipeline()
{
    if (graphicsPipeline) return graphicsPipeline;
    if (computePipeline) return computePipeline;

    return nullptr;
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
        .y = val.y,
        .width = val.width,
        .height = val.height,
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
    std::copy(other.finishSemaphores.begin(), other.finishSemaphores.end(),
        std::back_inserter(waitSemaphores));
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
