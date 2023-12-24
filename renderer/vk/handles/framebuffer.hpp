#pragma once

#include "device.hpp"

namespace vk { namespace handles {

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

class Framebuffer : public Handle<VkFramebuffer>
{
    HANDLE(Framebuffer);

public:
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer(const Device& device, FramebufferCreateInfo createInfo) noexcept;
    virtual ~Framebuffer();

protected:
    Framebuffer(
        const Device& device, FramebufferCreateInfo createInfo, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
