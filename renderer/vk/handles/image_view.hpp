#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageSubresourceRange)
    VKSTRUCT_PROPERTY(VkImageAspectFlags, aspectMask)
    VKSTRUCT_PROPERTY(uint32_t, baseMipLevel)
    VKSTRUCT_PROPERTY(uint32_t, levelCount)
    VKSTRUCT_PROPERTY(uint32_t, baseArrayLayer)
    VKSTRUCT_PROPERTY(uint32_t, layerCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(ImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkImageViewCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkImage, image)
    VKSTRUCT_PROPERTY(VkImageViewType, viewType)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkComponentMapping, components)
    VKSTRUCT_PROPERTY(ImageSubresourceRange, subresourceRange)
END_DECLARE_VKSTRUCT()

class Device;

class ImageView : public Handle<VkImageView>
{
    HANDLE(ImageView);

public:
    ImageView(const ImageView& other) = delete;
    ImageView(ImageView&& other) noexcept;
    ImageView(const Device& device, ImageViewCreateInfo createInfo) noexcept;
    virtual ~ImageView();

protected:
    ImageView(
        const Device& device, ImageViewCreateInfo createInfo, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
