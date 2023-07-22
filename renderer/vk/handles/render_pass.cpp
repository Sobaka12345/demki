#include "render_pass.hpp"

#include "device.hpp"

namespace vk { namespace handles {

RenderPassCreateInfo RenderPass::defaultCreateInfo()
{
	return {};
}

RenderPass::RenderPass(RenderPass &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

RenderPass::RenderPass(
    const Device &device, RenderPassCreateInfo createInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateRenderPass, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create render pass!");
}

RenderPass::~RenderPass()
{
    destroy(vkDestroyRenderPass, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
