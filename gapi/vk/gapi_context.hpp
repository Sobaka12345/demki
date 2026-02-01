#ifndef VK_GAPI_CONTEXT_HPP
#define VK_GAPI_CONTEXT_HPP

#include "../gapi_fwd.hpp"

#include <vulkan/vulkan.h>

namespace gapi::__private {

template<>
struct GApiContext<Vk> {
    struct QueueFamily {
        enum Type : uint16_t
        {
            BEGIN = 0,
            GRAPHICS_COMPUTE = BEGIN,
            PRESENT,
            COUNT,
            INVALID_QUEUE_FAMILY_INDEX = COUNT
        };

        uint32_t index = Type::INVALID_QUEUE_FAMILY_INDEX;
        VkQueueFamilyProperties properties;
    };

    struct PhysicalDevice
    {
        VkPhysicalDevice handle;

        std::array<QueueFamily, QueueFamily::Type::COUNT> queueFamilies;

        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VkPhysicalDeviceProperties properties;
        std::vector<VkExtensionProperties> availableExtensions;
        
        struct SurfaceInfo {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        } surfaceInfo;
    };

    VkInstance instance;
    VkSurfaceKHR surface;

    std::vector<PhysicalDevice> physicalDevices;
    void fetchPhysicalDevices() noexcept;
};

}

#endif // VK_GAPI_CONTEXT_HPP