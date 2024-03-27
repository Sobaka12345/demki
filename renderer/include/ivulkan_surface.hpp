#pragma once

#include "isurface.hpp"

#include "../vk/graphics_context.hpp"
#include "../vk/handles/instance.hpp"

#include <vulkan/vulkan_core.h>

namespace renderer {

class IVulkanSurface : virtual public ISurface
{
public:
    virtual VkSurfaceKHR surfaceKHR() const = 0;

protected:
    vk::GraphicsContext* createContext(vk::handles::ApplicationInfo appInfo)
    {
        return new vk::GraphicsContext(appInfo);
    }
};

}    //  namespace renderer
