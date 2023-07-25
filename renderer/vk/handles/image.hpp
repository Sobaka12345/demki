#pragma once

#include "simemory_accessor.hpp"

namespace vk {

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageSubresourceLayers)
    VKSTRUCT_PROPERTY(VkImageAspectFlags, aspectMask)
    VKSTRUCT_PROPERTY(uint32_t, mipLevel)
    VKSTRUCT_PROPERTY(uint32_t, baseArrayLayer)
    VKSTRUCT_PROPERTY(uint32_t, layerCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageSubresourceRange)
    VKSTRUCT_PROPERTY(VkImageAspectFlags, aspectMask)
    VKSTRUCT_PROPERTY(uint32_t, baseMipLevel)
    VKSTRUCT_PROPERTY(uint32_t, levelCount)
    VKSTRUCT_PROPERTY(uint32_t, baseArrayLayer)
    VKSTRUCT_PROPERTY(uint32_t, layerCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(ImageMemoryBarrier, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkAccessFlags, srcAccessMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, dstAccessMask)
    VKSTRUCT_PROPERTY(VkImageLayout, oldLayout)
    VKSTRUCT_PROPERTY(VkImageLayout, newLayout)
    VKSTRUCT_PROPERTY(uint32_t, srcQueueFamilyIndex)
    VKSTRUCT_PROPERTY(uint32_t, dstQueueFamilyIndex)
    VKSTRUCT_PROPERTY(VkImage, image)
    VKSTRUCT_PROPERTY(ImageSubresourceRange, subresourceRange)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageBlit)
    VKSTRUCT_PROPERTY(ImageSubresourceLayers, srcSubresource)
    VKSTRUCT_PROPERTY(std::span<const VkOffset3D COMMA 2>, srcOffsets)
    VKSTRUCT_PROPERTY(ImageSubresourceLayers, dstSubresource)
    VKSTRUCT_PROPERTY(std::span<const VkOffset3D COMMA 2>, dstOffsets)
END_DECLARE_VKSTRUCT()

namespace handles {

BEGIN_DECLARE_VKSTRUCT(ImageCreateInfo, VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkImageCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkImageType, imageType)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkExtent3D, extent)
    VKSTRUCT_PROPERTY(uint32_t, mipLevels)
    VKSTRUCT_PROPERTY(uint32_t, arrayLayers)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, samples)
    VKSTRUCT_PROPERTY(VkImageTiling, tiling)
    VKSTRUCT_PROPERTY(VkImageUsageFlags, usage)
    VKSTRUCT_PROPERTY(VkSharingMode, sharingMode)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndexCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pQueueFamilyIndices)
    VKSTRUCT_PROPERTY(VkImageLayout, initialLayout)
END_DECLARE_VKSTRUCT()

class Image
    : public SIMemoryAccessor<Image>
    , public Handle<VkImage>
{
public:
    Image(Image&& other) noexcept;
    Image(const Image& other) = delete;
    Image(const Device& device, VkHandleType* handle);
    Image(const Device& device, ImageCreateInfo imageInfo, VkHandleType* handlePtr = nullptr);
    ~Image();

    bool bindMemory(uint32_t bindingOffset);

    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
	{
        return allocateMemoryImpl(properties);
	}

    void transitionLayout(
        VkImageLayout oldLayout, VkImageLayout newLayout, ImageSubresourceRange subresourceRange);

private:
    std::weak_ptr<Memory> allocateMemoryImpl(VkMemoryPropertyFlags properties);
};

}    //  namespace handles
}    //  namespace vk
