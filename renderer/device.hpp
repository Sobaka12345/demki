#pragma once

#include <set>
#include <array>
#include <vector>
#include <limits>
#include <optional>

#include <vulkan/vulkan_core.h>

namespace vk {

class Device
{
    static const std::array<const char* const, 1> s_deviceExtensions;

public:
    enum QueueFamilyType
    {
        GRAPHICS,
        PRESENT,
        COUNT
    };

    class QueueFamilies
    {
        static constexpr uint32_t s_invalidIndex = (std::numeric_limits<uint32_t>::max)();
    public:
        QueueFamilies();
        QueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

        bool isComplete() const;
        uint32_t queueFamilyIndex(QueueFamilyType type) const;
        uint32_t operator[](QueueFamilyType type) const { return queueFamilyIndex(type); }

        const auto begin() const { return m_queueFamilyIndices.begin(); };
        const auto end() const { return m_queueFamilyIndices.end(); };

    private:
        std::array<uint32_t, QueueFamilyType::COUNT> m_queueFamilyIndices = []() constexpr {
            std::array<uint32_t, QueueFamilyType::COUNT> result;
            for (int i = 0; i < QueueFamilyType::COUNT; i++) result[i] = s_invalidIndex;
            return result;
        }();
    };

public:
    Device();
    Device(Device&& other);
    Device(const Device& other) = delete;
    Device(VkInstance instance, VkSurfaceKHR surface);
    ~Device();

    operator VkDevice() const { return m_device; }

    QueueFamilies queueFamilies() const { return m_queueFamilies; };

    VkQueue queue(QueueFamilyType type, uint32_t idx) const;

    VkDevice device() const { return m_device; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    VkPhysicalDeviceProperties physicalDeviceProperties() const { return m_physicalDeviceProperties; }

private:
    void pickPhysicalDevice();
    void createLogicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);

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
