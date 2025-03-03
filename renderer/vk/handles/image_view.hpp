#pragma once

#include "handle.hpp"
#include "image.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(ImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkImageViewCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkImage, image)
    VKSTRUCT_PROPERTY(VkImageViewType, viewType)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkComponentMapping, components)
    VKSTRUCT_PROPERTY(ImageSubresourceRange, subresourceRange)
END_DECLARE_VKSTRUCT()

template <typename T>
struct ImageViewFunctions : public CRTPBase<T>
{
    CREATE_FUNC(ImageView, Device);
    DESTROY_FUNC(ImageView, Device);
};

template <typename T>
struct ImageViewGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(ImageView, ImageViewFunctions, ImageViewGroupFunctions);
}

}    //  namespace renderer::vk
