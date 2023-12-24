#pragma once

#include "utils.hpp"
#include "image.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(SwapchainCreateInfoKHR, VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkSwapchainCreateFlagsKHR, flags)
    VKSTRUCT_PROPERTY(VkSurfaceKHR, surface)
    VKSTRUCT_PROPERTY(uint32_t, minImageCount)
    VKSTRUCT_PROPERTY(VkFormat, imageFormat)
    VKSTRUCT_PROPERTY(VkColorSpaceKHR, imageColorSpace)
    VKSTRUCT_PROPERTY(VkExtent2D, imageExtent)
    VKSTRUCT_PROPERTY(uint32_t, imageArrayLayers)
    VKSTRUCT_PROPERTY(VkImageUsageFlags, imageUsage)
    VKSTRUCT_PROPERTY(VkSharingMode, imageSharingMode)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndexCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pQueueFamilyIndices)
    VKSTRUCT_PROPERTY(VkSurfaceTransformFlagBitsKHR, preTransform)
    VKSTRUCT_PROPERTY(VkCompositeAlphaFlagBitsKHR, compositeAlpha)
    VKSTRUCT_PROPERTY(VkPresentModeKHR, presentMode)
    VKSTRUCT_PROPERTY(VkBool32, clipped)
    VKSTRUCT_PROPERTY(VkSwapchainKHR, oldSwapchain)
END_DECLARE_VKSTRUCT()

class Device;

class Swapchain : public Handle<VkSwapchainKHR>
{
    HANDLE(Swapchain);

public:
    Swapchain(const Swapchain& other) = delete;
    Swapchain(Swapchain&& other) noexcept;
    Swapchain(const Device& device, SwapchainCreateInfoKHR createInfo) noexcept;
    virtual ~Swapchain();

    HandleVector<Image> images() const;

    VkExtent2D extent() const { return m_extent; }

    VkFormat imageFormat() const { return m_imageFormat; }

    const Device& device() const { return m_device; }

protected:
    Swapchain(
        const Device& device, SwapchainCreateInfoKHR createInfo, VkHandleType* handlePtr) noexcept;

public:
    const Device& m_device;
    VkExtent2D m_extent;
    VkFormat m_imageFormat;
};

}}    //  namespace vk::handles
