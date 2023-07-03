#include "image_view.hpp"

namespace vk { namespace handles {

ImageView::ImageView(ImageView&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

ImageView::ImageView(const Device& device, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{}

ImageView::ImageView(const Device& device, ImageViewCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateImageView, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create image view!");
}

ImageView::~ImageView()
{
    destroy(vkDestroyImageView, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
