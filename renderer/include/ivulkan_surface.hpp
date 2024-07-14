#pragma once

#include "isurface.hpp"

#include "../vk/graphics_context.hpp"
#include "../vk/handles/instance.hpp"

#include <vulkan/vulkan_core.h>

namespace renderer {

class IVulkanSurface : virtual public ISurface
{
protected:
    renderer::vk::GraphicsContext* createContext(const char* applicationName)
    {
        const auto appInfo = vk::handles::ApplicationInfo{}
            .pApplicationName(applicationName)
            .applicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .pEngineName("DemkiEngine")
            .engineVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .apiVersion(VK_API_VERSION_1_3);
        const auto extensions = requiredExtensions();

        return new vk::GraphicsContext(
            vk::handles::InstanceCreateInfo{}
            .enabledExtensionCount(extensions.size())
            .ppEnabledExtensionNames(extensions.data())
            .pApplicationInfo(&appInfo));
    }

public:
    virtual VkSurfaceKHR surfaceKHR() const = 0;
    virtual void waitEvents() const = 0;

protected:
    virtual std::vector<const char*> validationLayers() = 0;
    virtual std::vector<const char*> requiredExtensions() = 0;
};

}    //  namespace renderer
