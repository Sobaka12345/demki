#include "framebuffer.hpp"

namespace vk { namespace handles {

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Framebuffer::Framebuffer(
    const Device& device, FramebufferCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateFramebuffer, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create image view!");
}

Framebuffer::~Framebuffer()
{
    destroy(vkDestroyFramebuffer, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
