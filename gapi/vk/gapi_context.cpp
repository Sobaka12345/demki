#include "gapi_context.hpp"
#include "gapi_fwd.hpp"


namespace gapi::__private {

void GApiContext<Vk>::fetchPhysicalDevices() noexcept {
    uint32_t physicalDeviceCount = 0;
    ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr) == VK_SUCCESS);
    std::vector<VkPhysicalDevice> vkPhysicalDevices(physicalDeviceCount);
    ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, vkPhysicalDevices.data()) == VK_SUCCESS);

    physicalDevices.resize(physicalDeviceCount);

    for (size_t pdi = 0; pdi < vkPhysicalDevices.size(); ++pdi)
    {
        const VkPhysicalDevice vkPhysicalDevice = vkPhysicalDevices[pdi]; 

        PhysicalDevice& physicalDevice = physicalDevices[pdi];
        physicalDevice.handle = vkPhysicalDevice;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &physicalDevice.features);
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDevice.properties);
        vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &physicalDevice.memoryProperties);

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, surface, &physicalDevice.surfaceInfo.capabilities);

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
        physicalDevice.availableExtensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount,
            physicalDevice.availableExtensions.data());

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);
        if (formatCount)
        {
            physicalDevice.surfaceInfo.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount,
                physicalDevice.surfaceInfo.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount,
            nullptr);
        if (presentModeCount)
        {
            physicalDevice.surfaceInfo.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount,
                physicalDevice.surfaceInfo.presentModes.data());
        }

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount,
            vkQueueFamilyProperties.data());

        for (size_t qfi = 0; qfi < vkQueueFamilyProperties.size(); ++qfi)
        {
            VkQueueFamilyProperties p = vkQueueFamilyProperties[qfi];
            if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (p.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                physicalDevice.queueFamilies[QueueFamily::Type::GRAPHICS_COMPUTE].index = qfi;
                physicalDevice.queueFamilies[QueueFamily::Type::GRAPHICS_COMPUTE].properties = p;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, qfi, surface, &presentSupport);
            if (presentSupport)
            {
                
                physicalDevice.queueFamilies[QueueFamily::Type::PRESENT].index = qfi;
                physicalDevice.queueFamilies[QueueFamily::Type::PRESENT].properties = p;
            }
        }
    }
}

VkFormat GApiContext<Vk>::PhysicalDevice::findSupportedFormat(
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) 
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(handle, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ASSERT(false, "failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
}

}