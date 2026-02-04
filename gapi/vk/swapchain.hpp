#ifndef GAPI_VK_SWAPCHAIN_HPP
#define GAPI_VK_SWAPCHAIN_HPP

#include "../gapi_fwd.hpp"
#include <type_traits>
#include <vulkan/vulkan.h>

#include <vector>

namespace gapi::__private {

template<>
struct Swapchain<Vk> {
    uint32_t size = 0;
    VkExtent2D extent = {};
    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    VkSwapchainKHR handle = VK_NULL_HANDLE;
    uint32_t currentImage = 0;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};


} // namespace gapi

#endif // GAPI_VK_SWAPCHAIN_HPP
