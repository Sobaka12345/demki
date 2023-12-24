#include "swapchain.hpp"

#include "device.hpp"

namespace vk { namespace handles {

Swapchain::Swapchain(Swapchain&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Swapchain::Swapchain(
    const Device& device, SwapchainCreateInfoKHR createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
    , m_imageFormat(createInfo.imageFormat())
    , m_extent(createInfo.imageExtent())
{
    ASSERT(create(vkCreateSwapchainKHR, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create swapchain");
}

Swapchain::Swapchain(const Device& device, SwapchainCreateInfoKHR createInfo) noexcept
    : Swapchain(device, std::move(createInfo), nullptr)
{}

Swapchain::~Swapchain()
{
    destroy(vkDestroySwapchainKHR, m_device, handle(), nullptr);
}

HandleVector<Image> Swapchain::images() const
{
    return Image::swapChainImages(m_device, *this);
}


}}    //  namespace vk::handles
