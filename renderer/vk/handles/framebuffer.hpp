#pragma once

#include "device.hpp"

namespace vk { namespace handles {

class Framebuffer : public Handle<VkFramebuffer>
{
public:
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer(const Device& device,
        VkFramebufferCreateInfo createInfo,
        VkHandleType* handlePtr = nullptr);
    ~Framebuffer();

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
