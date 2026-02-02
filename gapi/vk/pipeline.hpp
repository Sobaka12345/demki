#ifndef GAPI_VK_PIPELINE_HPP
#define GAPI_VK_PIPELINE_HPP

#include "../gapi_fwd.hpp"

#include <vulkan/vulkan.h>

namespace gapi::__private {

template <>
struct Pipeline<Vk>
{
    VkPipeline handle;
};

}

#endif // GAPI_VK_PIPELINE_HPP