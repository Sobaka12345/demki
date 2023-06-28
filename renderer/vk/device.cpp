#include "device.hpp"

#include "utils.hpp"
#include "command_pool.hpp"
#include "creators.hpp"
#include "graphical_application.hpp"
#include "queue.hpp"

#include <vector>
#include <functional>
#include <stdexcept>

namespace vk {

const std::array<const char* const, 1> Device::s_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

QueueFamilies::QueueFamilies()
{
    constexpr uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();
    m_queueFamilyIndices.fill(invalidIndex);
}

QueueFamilies::QueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

    constexpr uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();
    m_queueFamilyIndices.fill(invalidIndex);

    for (int i = 0; i < queueFamilyProperties.size(); ++i)
    {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_queueFamilyIndices[QueueFamilyType::GRAPHICS] = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            m_queueFamilyIndices[QueueFamilyType::PRESENT] = i;
        }
    }
}

bool QueueFamilies::isComplete() const
{
    return std::find(m_queueFamilyIndices.begin(), m_queueFamilyIndices.end(), s_invalidIndex) == m_queueFamilyIndices.end();
}

uint32_t QueueFamilies::queueFamilyIndex(QueueFamilyType type) const
{
    return m_queueFamilyIndices[type];
}

Device::Device()
    : Handle(nullptr)
    , m_instance(VK_NULL_HANDLE)
    , m_physicalDevice(VK_NULL_HANDLE)
    , m_surface(VK_NULL_HANDLE)
{
}

Device::Device(Device &&other)
    : Handle(std::move(other))
    , m_instance(other.m_instance)
    , m_surface(other.m_surface)
    , m_physicalDevice(std::move(other.m_physicalDevice))
    , m_physicalDeviceFeatures(std::move(other.m_physicalDeviceFeatures))
    , m_queueFamilies(std::move(other.m_queueFamilies))
    , m_physicalDeviceProperties(std::move(other.m_physicalDeviceProperties))
{
}

Device::Device(VkInstance instance, VkSurfaceKHR surface, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_instance(instance)
    , m_surface(surface)
{
    pickPhysicalDevice();
    createLogicalDevice();
}

Device::~Device()
{
    m_commandPools.clear();
    destroy(vkDestroyDevice, handle(), nullptr);
}

void Device::waitIdle() const
{
    vkDeviceWaitIdle(handle());
}

void Device::pickPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

    ASSERT(physicalDeviceCount, "failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());
    if (auto iter = std::find_if(physicalDevices.begin(), physicalDevices.end(),
            std::bind(std::mem_fn(&Device::isDeviceSuitable), this, std::placeholders::_1));
        iter == physicalDevices.end())
    {
        ASSERT(false, "failed to find a suitable GPU!");
    }
    else
    {
        m_physicalDevice = *iter;
        vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);
        vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
    }
}

void Device::createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> queueFamiliesSet(m_queueFamilies.begin(), m_queueFamilies.end());
    const std::array<float, 1> queuePriorities{1.0f};
    for (uint32_t queueFamilyIndex : queueFamiliesSet)
    {
        queueCreateInfos.push_back(create::deviceQueueCreateInfo(
            queueFamilyIndex, queuePriorities));
    }

    VkDeviceCreateInfo createInfo =
        GraphicalApplication::s_enableValidationLayers ?
            create::deviceCreateInfo(queueCreateInfos, s_deviceExtensions,
                GraphicalApplication::s_validationLayers) :
            create::deviceCreateInfo(queueCreateInfos, s_deviceExtensions);

    ASSERT(create(vkCreateDevice, m_physicalDevice, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create logical device!");
}

bool Device::isDeviceSuitable(VkPhysicalDevice device)
{
    if (!checkDeviceExtensionSupport(device))
    {
        return false;
    }

    const auto swapChainSupportDetails = utils::swapChainSupportDetails(device, m_surface);
    const bool swapChainAdequate =
        !swapChainSupportDetails.formats.empty() &&
        !swapChainSupportDetails.presentModes.empty();

    m_queueFamilies = QueueFamilies(device, m_surface);

    return swapChainAdequate && m_queueFamilies.isComplete();
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(s_deviceExtensions.begin(), s_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

std::weak_ptr<Queue> Device::queue(QueueFamilyType type, uint32_t idx) const
{
    const uint32_t familyIdx = m_queueFamilies.queueFamilyIndex(type);
    auto it = m_queues.find({ familyIdx, idx });
    if (it == m_queues.end())
    {
        auto [res, _] = m_queues.emplace(std::pair{ std::pair{familyIdx, idx},
            std::make_shared<Queue>(*this, familyIdx, idx) });
        return res->second;
    }

    return it->second;
}

std::weak_ptr<CommandPool> Device::commandPool(QueueFamilyType type) const
{
    const uint32_t familyIdx = m_queueFamilies.queueFamilyIndex(type);
    auto it = m_commandPools.find(familyIdx);
    if (it == m_commandPools.end())
    {
        auto [res, _] = m_commandPools.emplace(std::pair{ familyIdx, std::make_shared<CommandPool>(*this,
            create::commandPoolCreateInfo(
                familyIdx, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) });
        return res->second;
    }
    
    return it->second;
}

OneTimeCommand Device::oneTimeCommand(QueueFamilyType type, uint32_t queueIdx) const
{
    return OneTimeCommand{ *this, type, queueIdx };
}

}
