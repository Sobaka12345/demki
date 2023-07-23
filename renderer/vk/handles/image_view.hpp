#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(ImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkImageViewCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkImage, image)
    VKSTRUCT_PROPERTY(VkImageViewType, viewType)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkComponentMapping, components)
    VKSTRUCT_PROPERTY(VkImageSubresourceRange, subresourceRange)
END_DECLARE_VKSTRUCT()

class Device;

class ImageView : public Handle<VkImageView>
{
public:
    ImageView(const ImageView& other) = delete;
    ImageView(ImageView&& other) noexcept;
    ImageView(const Device& device, VkHandleType* handlePtr) noexcept;
    ImageView(
        const Device& device, ImageViewCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~ImageView();

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
