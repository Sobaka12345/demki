#pragma once

#include "iwindow.hpp"

#include <vulkan/vulkan_core.h>

namespace renderer::vk {
class GraphicsContext;
}

namespace shell {

class IVulkanWindow : virtual public IWindow
{
public:
    virtual VkSurfaceKHR surfaceKHR() const = 0;

private:
    virtual void init(VkInstance instance) = 0;
    virtual void destroy() = 0;

    friend class renderer::vk::GraphicsContext;
};

}
