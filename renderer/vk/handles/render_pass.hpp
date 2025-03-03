#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(RenderPassCreateInfo, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkRenderPassCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, attachmentCount)
    VKSTRUCT_PROPERTY(const VkAttachmentDescription*, pAttachments)
    VKSTRUCT_PROPERTY(uint32_t, subpassCount)
    VKSTRUCT_PROPERTY(const VkSubpassDescription*, pSubpasses)
    VKSTRUCT_PROPERTY(uint32_t, dependencyCount)
    VKSTRUCT_PROPERTY(const VkSubpassDependency*, pDependencies)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(RenderPassBeginInfo, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkRenderPass, renderPass)
    VKSTRUCT_PROPERTY(VkFramebuffer, framebuffer)
    VKSTRUCT_PROPERTY(VkRect2D, renderArea)
    VKSTRUCT_PROPERTY(uint32_t, clearValueCount)
    VKSTRUCT_PROPERTY(const VkClearValue*, pClearValues)
END_DECLARE_VKSTRUCT()

template <typename T>
struct RenderPassFunctions : public CRTPBase<T>
{
    CREATE_FUNC(RenderPass, Device)
    DESTROY_FUNC(RenderPass, Device)
};

template <typename T>
struct RenderPassGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(RenderPass, RenderPassFunctions, RenderPassGroupFunctions);
}

}    //  namespace renderer::vk
