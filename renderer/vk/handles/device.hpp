#pragma once

#include "command.hpp"
#include "handle.hpp"

#include <set>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include <memory>
#include <optional>

#include <vulkan/vulkan_core.h>

namespace vk { namespace handles {

class Queue;
class CommandPool;

enum QueueFamilyType : uint16_t
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

class Device : public Handle<VkDevice>
{
    static const std::array<const char* const, 1> s_deviceExtensions;

public:
    Device();
    Device(Device&& other);
    Device(VkInstance instance, VkSurfaceKHR surface, VkHandleType* handlePtr = nullptr);
    ~Device();

    void waitIdle() const;

    QueueFamilies queueFamilies() const { return m_queueFamilies; };

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
};

}}    //  namespace vk::handles
