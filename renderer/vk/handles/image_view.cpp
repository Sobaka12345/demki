#include "image_view.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {

ImageView::ImageView(ImageView&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

ImageView::ImageView(
    const Device& device, ImageViewCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateImageView, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create image view!");
}

ImageView::ImageView(const Device& device, ImageViewCreateInfo createInfo) noexcept
    : ImageView(device, std::move(createInfo), nullptr)
{}

ImageView::~ImageView()
{
    destroy(vkDestroyImageView, m_device, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles
