#include "render_pass.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {

RenderPass::RenderPass(RenderPass&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_attachments(std::move(other.m_attachments))
{}

RenderPass::RenderPass(
    const Device& device, RenderPassCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
    , m_attachments(createInfo.pAttachments(),
          createInfo.pAttachments() + createInfo.attachmentCount())
{
    ASSERT(create(vkCreateRenderPass, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create render pass!");
}

RenderPass::RenderPass(const Device& device, RenderPassCreateInfo createInfo) noexcept
    : RenderPass(device, std::move(createInfo), nullptr)
{}

RenderPass::~RenderPass()
{
    destroy(vkDestroyRenderPass, m_device, handle(), nullptr);
}

std::span<const AttachmentDescription> RenderPass::attachments()
{
    return m_attachments;
}

}}    //  namespace renderer::vk::handles
