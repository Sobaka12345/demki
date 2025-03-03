#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(FramebufferCreateInfo, VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkFramebufferCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkRenderPass, renderPass)
    VKSTRUCT_PROPERTY(uint32_t, attachmentCount)
    VKSTRUCT_PROPERTY(const VkImageView*, pAttachments)
    VKSTRUCT_PROPERTY(uint32_t, width)
    VKSTRUCT_PROPERTY(uint32_t, height)
    VKSTRUCT_PROPERTY(uint32_t, layers)
END_DECLARE_VKSTRUCT()

template <typename T>
struct FramebufferFunctions : public CRTPBase<T>
{
    CREATE_FUNC(Framebuffer, Device)
    DESTROY_FUNC(Framebuffer, Device)
};

template <typename T>
struct FramebufferGroupFunctions : public CRTPBase<T>
{};

namespace handles {

DECLARE_HANDLE_TYPE(Framebuffer, FramebufferFunctions, FramebufferGroupFunctions);
}

}    //  namespace renderer::vk
