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
    , m_attachments(std::move(other.m_attachments))
{}

RenderPass::RenderPass(
    const Device &device, RenderPassCreateInfo createInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_attachments(createInfo.pAttachments(),
          createInfo.pAttachments() + createInfo.attachmentCount())
{
    ASSERT(create(vkCreateRenderPass, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create render pass!");
}

RenderPass::~RenderPass()
{
    destroy(vkDestroyRenderPass, m_device, handle(), nullptr);
}

std::span<const AttachmentDescription> RenderPass::attachments()
{
    return m_attachments;
}

}}    //  namespace vk::handles
