#include "device.hpp"

#include "utils.hpp"
#include "creators.hpp"
#include "graphical_application.hpp"

#include <vector>
#include <functional>
#include <stdexcept>

namespace vk {

const std::array<const char* const, 1> Device::s_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Device::Device()
    : m_instance(VK_NULL_HANDLE)
    , m_device(VK_NULL_HANDLE)
    , m_physicalDevice(VK_NULL_HANDLE)
    , m_surface(VK_NULL_HANDLE)
{

}

Device::Device(Device &&other)
    : m_instance(std::move(other.m_instance))
    , m_surface(std::move(other.m_surface))
    , m_device(std::move(other.m_device))
    , m_physicalDevice(std::move(other.m_physicalDevice))
    , m_physicalDeviceFeatures(std::move(other.m_physicalDeviceFeatures))
    , m_queueFamilies(std::move(other.m_queueFamilies))
    , m_physicalDeviceProperties(std::move(other.m_physicalDeviceProperties))
{
    other.m_device = VK_NULL_HANDLE;
}

Device::Device(VkInstance instance, VkSurfaceKHR surface)
    : m_instance(instance)
    , m_surface(surface)
{
    pickPhysicalDevice();
    createLogicalDevice();
}

Device::~Device()
{
    vkDestroyDevice(m_device, nullptr);
}

void Device::pickPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

    if (!physicalDeviceCount)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());
    if (auto iter = std::find_if(physicalDevices.begin(), physicalDevices.end(),
            std::bind(std::mem_fn(&Device::isDeviceSuitable), this, std::placeholders::_1));
        iter == physicalDevices.end())
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    else
    {
        m_physicalDevice = *iter;
    }
}

void Device::createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto& queueFamily : m_queueFamilies.uniqueQueueFamilies())
    {
        queueCreateInfos.push_back(create::deviceQueueCreateInfo(
            queueFamily, std::array<float, 1>{1.0f}));
    }

    VkDeviceCreateInfo createInfo =
        GraphicalApplication::s_enableValidationLayers ?
            create::deviceCreateInfo(queueCreateInfos, s_deviceExtensions,
                GraphicalApplication::s_validationLayers) :
            create::deviceCreateInfo(queueCreateInfos, s_deviceExtensions);

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }
}

bool Device::isDeviceSuitable(VkPhysicalDevice device)
{
    vkGetPhysicalDeviceFeatures(device, &m_physicalDeviceFeatures);
    vkGetPhysicalDeviceProperties(device, &m_physicalDeviceProperties);

    const bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        const auto swapChainSupportDetails = utils::swapChainSupportDetails(device, m_surface);
        swapChainAdequate =
            !swapChainSupportDetails.formats.empty() &&
            !swapChainSupportDetails.presentModes.empty();

        m_queueFamilies = queueFamilies(device, m_surface);
    }

    return extensionsSupported
           && swapChainAdequate
           && m_queueFamilies.isComplete();
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

Device::QueueFamilies Device::queueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilies result;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (int i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            result.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            result.presentFamily = i;
        }

        if (result.isComplete())
        {
            return result;
        }
    }

    return result;
}

}
