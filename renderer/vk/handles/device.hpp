#pragma once

#include "command.hpp"
#include "handle.hpp"

#include <array>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DeviceCreateInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void *, pNext)
    VKSTRUCT_PROPERTY(VkDeviceCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, queueCreateInfoCount)
    VKSTRUCT_PROPERTY(const VkDeviceQueueCreateInfo *, pQueueCreateInfos)
    VKSTRUCT_PROPERTY(uint32_t, enabledLayerCount)
    VKSTRUCT_PROPERTY(const char *const *, ppEnabledLayerNames)
    VKSTRUCT_PROPERTY(uint32_t, enabledExtensionCount)
    VKSTRUCT_PROPERTY(const char *const *, ppEnabledExtensionNames)
    VKSTRUCT_PROPERTY(const VkPhysicalDeviceFeatures *, pEnabledFeatures)
END_DECLARE_VKSTRUCT()

class Queue;
class CommandPool;

enum QueueFamilyType : uint16_t
{
    GRAPHICS_COMPUTE,
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

class Device : public Handle<VkDevice>
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static const std::array<const char *const, 1> s_deviceExtensions;
    static SwapChainSupportDetails swapChainSupportDetails(VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);

public:
    Device();
    Device(Device &&other);
    Device(VkInstance instance, VkSurfaceKHR surface, VkHandleType *handlePtr = nullptr);
    ~Device();

    void waitIdle() const;

    QueueFamilies queueFamilies() const { return m_queueFamilies; };

    VkMemoryType memoryType(uint32_t index) const;

    std::weak_ptr<Queue> queue(QueueFamilyType type, uint32_t idx = 0) const;
    std::weak_ptr<CommandPool> commandPool(QueueFamilyType type) const;
    OneTimeCommand oneTimeCommand(QueueFamilyType type, uint32_t queueIdx = 0) const;

    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }

    VkPhysicalDeviceProperties physicalDeviceProperties() const
    {
        return m_physicalDeviceProperties;
    }

private:
    void pickPhysicalDevice();
    void createLogicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);

private:
    const VkInstance m_instance;
    const VkSurfaceKHR m_surface;

    QueueFamilies m_queueFamilies;
    mutable std::map<std::pair<uint32_t, uint32_t>, std::shared_ptr<Queue>> m_queues;
    mutable std::map<uint32_t, std::shared_ptr<CommandPool>> m_commandPools;

    //  TO DO: Support for multiple devices
    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
    VkPhysicalDeviceProperties m_physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
};

}}    //  namespace vk::handles
