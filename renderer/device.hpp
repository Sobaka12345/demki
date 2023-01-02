#pragma once

#include <set>
#include <array>
#include <optional>

#include <vulkan/vulkan_core.h>

namespace vk {

class Device
{
    static const std::array<const char* const, 1> s_deviceExtensions;

public:
    struct QueueFamilies
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        std::set<uint32_t> uniqueQueueFamilies() const
        {
            return isComplete() ?
                std::set<uint32_t>{ presentFamily.value(), graphicsFamily.value() } :
                std::set<uint32_t>{};
        }

        bool isComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

public:
    Device();
    Device(Device&& other);
    Device(const Device& other) = delete;
    Device(VkInstance instance, VkSurfaceKHR surface);
    ~Device();

    operator VkDevice() const { return m_device; }

    VkDevice device() const { return m_device; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VkPhysicalDeviceProperties physicalDeviceProperties() const { return m_physicalDeviceProperties; }
    const QueueFamilies& queueFamilies() const { return m_queueFamilies; }

private:
    void pickPhysicalDevice();
    void createLogicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    static QueueFamilies queueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    const VkInstance m_instance;
    const VkSurfaceKHR m_surface;
    // TO DO: Support for multiple devices
    VkDevice m_device;
    QueueFamilies m_queueFamilies;
    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
    VkPhysicalDeviceProperties m_physicalDeviceProperties;
};

}
