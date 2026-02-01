#ifndef GAPI_VK_SURFACE_HPP
#define GAPI_VK_SURFACE_HPP

#include "../gapi_fwd.hpp"
#include <vulkan/vulkan_core.h>

namespace gapi::__private {

template <>
struct Surface<Vk> {
    VkSurfaceKHR handle;
};

}

#endif // GAPI_VK_SURFACE_HPP