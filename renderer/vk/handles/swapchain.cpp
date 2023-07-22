#include "swapchain.hpp"

#include "device.hpp"

namespace vk { namespace handles {

Swapchain::Swapchain(Swapchain &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Swapchain::Swapchain(
    const Device &device, SwapchainCreateInfoKHR createInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_imageFormat(createInfo.imageFormat())
    , m_extent(createInfo.imageExtent())
{
    ASSERT(create(vkCreateSwapchainKHR, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create swapchain");
}

Swapchain::~Swapchain()
{
    destroy(vkDestroySwapchainKHR, m_device, handle(), nullptr);
}

HandleVector<Image> Swapchain::images() const
{
    uint32_t imageCount;
    HandleVector<Image> result;

    vkGetSwapchainImagesKHR(m_device, handle(), &imageCount, nullptr);
    result.resize(imageCount, m_device);
    vkGetSwapchainImagesKHR(m_device, handle(), &imageCount, result.handleData());

    return result;
}


}}    //  namespace vk::handles
