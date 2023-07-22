#include "render_context.hpp"

#include "handles/command_buffer.hpp"
#include "handles/render_pass.hpp"

#include <vulkan/vulkan_core.h>

namespace vk {

inline VkViewport toVkViewport(const Viewport &val)
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

inline VkRect2D toVkScissors(const Scissors &scissors)
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

RenderContext::~RenderContext()
{
    if (commandBuffer) vkCmdEndRenderPass(*commandBuffer);
}

void RenderContext::setScissors(Scissors scissors) const
{
    commandBuffer->setScissor(toVkScissors(scissors));
}

void RenderContext::setViewport(Viewport viewport) const
{
    commandBuffer->setViewport(toVkViewport(viewport));
}

}    //  namespace vk
