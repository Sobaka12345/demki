#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

enum class QueueFamilyType : uint16_t
{
    BEGIN = 0,
    GRAPHICS_COMPUTE = BEGIN,
    PRESENT,
    COUNT,
    INVALID_QUEUE_FAMILY_INDEX = COUNT
};

struct PhysicalDeviceInfo
{
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkPhysicalDeviceProperties properties;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::array<uint32_t, enumT(QueueFamilyType::COUNT)> queueFamilyIndices;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> surfacePresentModes;
};

template <typename T>
struct PhysicalDeviceFunctions : public CRTPBase<T>
{
    static constexpr inline void enumerate(VkInstance instance,
        uint32_t* pPhysicalDeviceCount,
        VkPhysicalDevice* pPhysicalDevices) noexcept
    {
        ASSERT(vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices) ==
            VK_SUCCESS);
    }

    static inline PhysicalDeviceInfo info(VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface) noexcept
    {
        PhysicalDeviceInfo result;
        vkGetPhysicalDeviceFeatures(physicalDevice, &result.features);
        vkGetPhysicalDeviceProperties(physicalDevice, &result.properties);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &result.memoryProperties);
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
            &result.surfaceCapabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount)
        {
            result.surfaceFormats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                result.surfaceFormats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount,
            nullptr);
        if (presentModeCount)
        {
            result.surfacePresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount,
                result.surfacePresentModes.data());
        }

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        if (queueFamilyCount)
        {
            result.queueFamilyProperties.resize(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                result.queueFamilyProperties.data());
        }

        result.queueFamilyIndices.fill(enumT(QueueFamilyType::INVALID_QUEUE_FAMILY_INDEX));

        for (int i = 0; i < result.queueFamilyProperties.size(); ++i)
        {
            if ((result.queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (result.queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                result.queueFamilyIndices[enumT(QueueFamilyType::GRAPHICS_COMPUTE)] = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport)
            {
                result.queueFamilyIndices[enumT(QueueFamilyType::PRESENT)] = i;
            }
        }

        return result;
    }

    static inline VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) noexcept
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
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
};

template <typename T>
struct PhysicalDeviceGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(PhysicalDevice, PhysicalDeviceFunctions, PhysicalDeviceGroupFunctions);
}

}    //  namespace renderer::vk
