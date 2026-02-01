#ifndef GAPI_OGL_SURFACE_HPP
#define GAPI_OGL_SURFACE_HPP

#include "../gapi_fwd.hpp"
#include <vulkan/vulkan_core.h>

namespace gapi::__private {

template <>
struct Surface<Ogl> {
    uint32_t handle;
};

}

#endif // GAPI_OGL_SURFACE_HPP