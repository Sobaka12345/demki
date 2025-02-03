#pragma once

#include "iwindow.hpp"

#include <ivulkan_surface.hpp>

namespace shell {

class IVulkanWindow
    : virtual public IWindow
    , public renderer::IVulkanSurface
{};

}
