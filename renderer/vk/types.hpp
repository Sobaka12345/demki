#pragma once

#include <array>
#include <concepts>

#include "vk/utils.hpp"

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(DeviceQueueCreateInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDeviceQueueCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndex)
    VKSTRUCT_PROPERTY(uint32_t, queueCount)
    VKSTRUCT_PROPERTY(const float*, pQueuePriorities)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(BufferCopy)
    VKSTRUCT_PROPERTY(VkDeviceSize, srcOffset)
    VKSTRUCT_PROPERTY(VkDeviceSize, dstOffset)
    VKSTRUCT_PROPERTY(VkDeviceSize, size)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(BufferImageCopy)
    VKSTRUCT_PROPERTY(VkDeviceSize, bufferOffset)
    VKSTRUCT_PROPERTY(uint32_t, bufferRowLength)
    VKSTRUCT_PROPERTY(uint32_t, bufferImageHeight)
    VKSTRUCT_PROPERTY(VkImageSubresourceLayers, imageSubresource)
    VKSTRUCT_PROPERTY(VkOffset3D, imageOffset)
    VKSTRUCT_PROPERTY(VkExtent3D, imageExtent)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorBufferInfo)
    VKSTRUCT_PROPERTY(VkBuffer, buffer)
    VKSTRUCT_PROPERTY(VkDeviceSize, offset)
    VKSTRUCT_PROPERTY(VkDeviceSize, range)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorImageInfo)
    VKSTRUCT_PROPERTY(VkSampler, sampler)
    VKSTRUCT_PROPERTY(VkImageView, imageView)
    VKSTRUCT_PROPERTY(VkImageLayout, imageLayout)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(Extent2D)
    VKSTRUCT_PROPERTY(uint32_t, width)
    VKSTRUCT_PROPERTY(uint32_t, height)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(Extent3D)
    VKSTRUCT_PROPERTY(uint32_t, width)
    VKSTRUCT_PROPERTY(uint32_t, height)
    VKSTRUCT_PROPERTY(uint32_t, depth)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(Offset2D)
    VKSTRUCT_PROPERTY(int32_t, x)
    VKSTRUCT_PROPERTY(int32_t, y)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(Offset3D)
    VKSTRUCT_PROPERTY(int32_t, x)
    VKSTRUCT_PROPERTY(int32_t, y)
    VKSTRUCT_PROPERTY(int32_t, z)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(AttachmentDescription)
    VKSTRUCT_PROPERTY(VkAttachmentDescriptionFlags, flags)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, samples)
    VKSTRUCT_PROPERTY(VkAttachmentLoadOp, loadOp)
    VKSTRUCT_PROPERTY(VkAttachmentStoreOp, storeOp)
    VKSTRUCT_PROPERTY(VkAttachmentLoadOp, stencilLoadOp)
    VKSTRUCT_PROPERTY(VkAttachmentStoreOp, stencilStoreOp)
    VKSTRUCT_PROPERTY(VkImageLayout, initialLayout)
    VKSTRUCT_PROPERTY(VkImageLayout, finalLayout)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(AttachmentReference)
    VKSTRUCT_PROPERTY(uint32_t, attachment)
    VKSTRUCT_PROPERTY(VkImageLayout, layout)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(SubpassDescription)
    VKSTRUCT_PROPERTY(VkSubpassDescriptionFlags, flags)
    VKSTRUCT_PROPERTY(VkPipelineBindPoint, pipelineBindPoint)
    VKSTRUCT_PROPERTY(uint32_t, inputAttachmentCount)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pInputAttachments)
    VKSTRUCT_PROPERTY(uint32_t, colorAttachmentCount)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pColorAttachments)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pResolveAttachments)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pDepthStencilAttachment)
    VKSTRUCT_PROPERTY(uint32_t, preserveAttachmentCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pPreserveAttachments)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(SubpassDependency)
    VKSTRUCT_PROPERTY(uint32_t, srcSubpass)
    VKSTRUCT_PROPERTY(uint32_t, dstSubpass)
    VKSTRUCT_PROPERTY(VkPipelineStageFlags, srcStageMask)
    VKSTRUCT_PROPERTY(VkPipelineStageFlags, dstStageMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, srcAccessMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, dstAccessMask)
    VKSTRUCT_PROPERTY(VkDependencyFlags, dependencyFlags)
END_DECLARE_VKSTRUCT()

}    //  namespace renderer::vk
