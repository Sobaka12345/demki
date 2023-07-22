#pragma once

#include "utils.hpp"

namespace vk { namespace handles {

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

class Device;

class RenderPass : public Handle<VkRenderPass>
{
public:
    static RenderPassCreateInfo defaultCreateInfo();

    RenderPass(const RenderPass& other) = delete;
    RenderPass(RenderPass&& other);
    RenderPass(
        const Device& device, RenderPassCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~RenderPass();

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
