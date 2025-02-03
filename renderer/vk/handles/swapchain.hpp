#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

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

template <typename T>
struct SwapchainKHRFunctions : public CRTPBase<T>
{};

template <typename T>
struct SwapchainKHRGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(SwapchainKHR, SwapchainKHRFunctions, SwapchainKHRGroupFunctions);
}

}    //  namespace renderer::vk::handles
