#include "graphics_context.hpp"

#include "handles/debug_utils_messenger.hpp"

#include "compute_pipeline.hpp"
#include "computer.hpp"
#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "storage_buffer.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
#include "uniform_buffer.hpp"

#include <operation_context.hpp>

#include <ivulkan_surface.hpp>
#include <iresources.hpp>

#include <cstring>
#include <iostream>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace renderer::vk {

using namespace handles;

static bool requiredExtensionsSupported(const std::vector<const char*>& required)
{
    uint32_t systemExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, nullptr);
    std::vector<VkExtensionProperties> systemExtensions(systemExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, systemExtensions.data());

    for (auto& required : required)
    {
        bool exists = false;
        for (auto& x : systemExtensions)
        {
            if (!std::strcmp(x.extensionName, required))
            {
                exists = true;
                break;
            }
        }

        if (exists == false)
        {
            return false;
        }
    }

    return true;
}

static bool requiredValidationLayerSupported(const std::span<const char* const> required)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto& required : required)
    {
        bool exists = false;
        for (auto& x : availableLayers)
        {
            if (!std::strcmp(x.layerName, required))
            {
                exists = true;
                break;
            }
        }

        if (exists == false)
        {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::string severity;
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: severity = "INFO: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: severity = "VERBOSE: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity = "WARNING: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: severity = "ERROR: "; break;
        default: severity = "WTF: ";
    }
    std::cout << severity << pCallbackData->messageIdNumber << ": " << pCallbackData->pMessageIdName
              << ":" << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

constexpr static auto s_debugMessengerCreateInfo =
    DebugUtilsMessengerCreateInfoEXT()
        .messageSeverity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        .messageType(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        .pfnUserCallback(debugCallback);

#ifdef NDEBUG
const bool GraphicsContext::s_enableValidationLayers = false;
#else
const bool GraphicsContext::s_enableValidationLayers = true;
#endif

const std::vector<const char*> GraphicsContext::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> GraphicsContext::s_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);    //!!!!!!!

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (GraphicsContext::s_enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
        availableExtensions.data());

    std::set<std::string> requiredExtensions(GraphicsContext::s_deviceExtensions.begin(),
        GraphicsContext::s_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

inline constexpr bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

GraphicsContext::GraphicsContext(ApplicationInfo appInfo)
{
    auto createInfo = InstanceCreateInfo().pApplicationInfo(&appInfo);

    if (s_enableValidationLayers)
    {
        ASSERT(requiredValidationLayerSupported(GraphicsContext::s_validationLayers),
            "required validation layers are absent");

        createInfo.enabledLayerCount(s_validationLayers.size())
            .ppEnabledLayerNames(s_validationLayers.data())
            .pNext(&s_debugMessengerCreateInfo);
        //  we need pNext to debug vk(Create|Destroy)Instance
    }
    else
    {
        createInfo.pNext(nullptr).enabledLayerCount(0);
    }

    const auto extensions = getRequiredExtensions();
    ASSERT(requiredExtensionsSupported(extensions), "required extensions are absent");

    createInfo.enabledExtensionCount(extensions.size()).ppEnabledExtensionNames(extensions.data());

    m_instance = Instance::create(&createInfo, nullptr, "failed to create instance ;c");

    if (s_enableValidationLayers)
    {
        m_debugMessenger =
            DebugUtilsMessengerEXT::create(m_instance, &s_debugMessengerCreateInfo, nullptr);
    }
}

GraphicsContext::~GraphicsContext()
{
    m_commandPools.destroyAll(m_device);
    Device::destroy(m_device, nullptr);
    DebugUtilsMessengerEXT::destroy(m_instance, m_debugMessenger, nullptr);
    Instance::destroy(m_instance, nullptr);
}

void GraphicsContext::init(IVulkanSurface& surface)
{
    uint32_t physicalDeviceCount = 0;
    PhysicalDevice::enumerate(m_instance, &physicalDeviceCount, nullptr);
    ASSERT(physicalDeviceCount, "failed to find GPUs with Vulkan support!");

    PhysicalDevice::Vector<> physicalDevices(physicalDeviceCount);
    PhysicalDevice::enumerate(m_instance, &physicalDeviceCount, physicalDevices.data());

    for (auto iter = physicalDevices.begin(); iter != physicalDevices.end(); ++iter)
    {
        const auto deviceInfo = PhysicalDevice::info(*iter, surface.surfaceKHR());
        constexpr uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();

        if (auto isSuitable = checkDeviceExtensionSupport(*iter) &&
                !deviceInfo.surfaceFormats.empty() && !deviceInfo.surfacePresentModes.empty() &&
                std::find(deviceInfo.queueFamilyIndices.begin(),
                    deviceInfo.queueFamilyIndices.end(),
                    enumT(QueueFamilyType::INVALID_QUEUE_FAMILY_INDEX)) ==
                    deviceInfo.queueFamilyIndices.end();
            isSuitable)
        {
            m_physicalDevices.emplace_back(*iter,
                PhysicalDevice::info(*iter, surface.surfaceKHR()));
        }
    }

    ASSERT(!m_physicalDevices.empty(), "failed to find a suitable GPU!");

    const std::array<float, 1> queuePriorities{ 1.0f };
    const auto& familyIndices = physicalDeviceInfo().queueFamilyIndices;
    std::array<DeviceQueueCreateInfo, enumT(QueueFamilyType::COUNT)> queueCreateInfos;

    for (QueueFamilyType i = QueueFamilyType::BEGIN; i < QueueFamilyType::COUNT; ++i)
    {
        queueCreateInfos[enumT(i)] =
            DeviceQueueCreateInfo{}
                .queueFamilyIndex(familyIndices[enumT(i)])
                .pQueuePriorities(queuePriorities.data())
                .queueCount(queuePriorities.size());
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    const auto createInfo = GraphicsContext::s_enableValidationLayers ?
        DeviceCreateInfo{}
            .pEnabledFeatures(&deviceFeatures)
            .queueCreateInfoCount(queueCreateInfos.size())
            .pQueueCreateInfos(queueCreateInfos.data())
            .enabledExtensionCount(s_deviceExtensions.size())
            .ppEnabledExtensionNames(s_deviceExtensions.data())
            .enabledLayerCount(GraphicsContext::s_validationLayers.size())
            .ppEnabledLayerNames(GraphicsContext::s_validationLayers.data()) :
        DeviceCreateInfo{}
            .pEnabledFeatures(&deviceFeatures)
            .pQueueCreateInfos(queueCreateInfos.data())
            .queueCreateInfoCount(queueCreateInfos.size())
            .enabledExtensionCount(s_deviceExtensions.size())
            .ppEnabledExtensionNames(s_deviceExtensions.data());

    m_device =
        Device::create(physicalDevice(), &createInfo, nullptr, "failed to create logical device!");

    for (QueueFamilyType i = QueueFamilyType::BEGIN; i < QueueFamilyType::COUNT; ++i)
    {
        uint32_t queueFamilyIndex = familyIndices[enumT(i)];
        auto commandPoolCreateInfo =
            CommandPoolCreateInfo{}
                .queueFamilyIndex(queueFamilyIndex)
                .flags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        m_commandPools[enumT(i)] = CommandPool::create(device(), &commandPoolCreateInfo, nullptr);
        m_queues[enumT(i)] = Queue::get(device(), familyIndices[enumT(i)], 0);
    }
}

VkFormat GraphicsContext::findDepthFormat() const
{
    return PhysicalDevice::findSupportedFormat(physicalDevice(),
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

uint32_t GraphicsContext::dynamicAlignment(uint32_t layoutSize) const
{
    static const uint32_t minAlignment =
        physicalDeviceInfo().properties.limits.minUniformBufferOffsetAlignment;

    if (minAlignment > 0)
    {
        layoutSize = (layoutSize + minAlignment - 1) & ~(minAlignment - 1);
    }
    return layoutSize;
}

VkInstance GraphicsContext::instance() const
{
    return m_instance;
}

VkDevice GraphicsContext::device() const
{
    return m_device;
}

uint32_t GraphicsContext::queueIndex(QueueFamilyType familyType) const
{
    return physicalDeviceInfo().queueFamilyIndices[enumT(familyType)];
}

VkQueue renderer::vk::GraphicsContext::queue(QueueFamilyType familyType) const
{
    return m_queues[enumT(familyType)];
}

VkPhysicalDevice GraphicsContext::physicalDevice() const
{
    return m_physicalDevices[0].first;
}

const PhysicalDeviceInfo& GraphicsContext::physicalDeviceInfo() const
{
    return m_physicalDevices[0].second;
}

VkCommandPool GraphicsContext::commandPool(QueueFamilyType type) const
{
    return m_commandPools[enumT(type)];
}

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(IVulkanSurface& surface,
    ISwapchain::CreateInfo createInfo)
{
    return std::make_shared<Swapchain>(*this, surface, std::move(createInfo));
}

std::shared_ptr<IComputer> GraphicsContext::createComputer(IComputer::CreateInfo createInfo)
{
    return std::make_shared<Computer>(*this, std::move(createInfo));
}

std::shared_ptr<IComputePipeline> GraphicsContext::createComputePipeline(
    IComputePipeline::CreateInfo createInfo)
{
    return std::make_shared<vk::ComputePipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IGraphicsPipeline> GraphicsContext::createGraphicsPipeline(
    IGraphicsPipeline::CreateInfo createInfo)
{
    return std::make_shared<vk::GraphicsPipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IRenderer> GraphicsContext::createRenderer(IRenderer::CreateInfo createInfo)
{
    return std::make_shared<Renderer>(*this, std::move(createInfo));
}

std::shared_ptr<IStorageBuffer> GraphicsContext::createStorageBuffer(
    IStorageBuffer::CreateInfo createInfo)
{
    return std::make_shared<StorageBuffer>(*this, std::move(createInfo));
}

std::shared_ptr<IUniformBuffer> GraphicsContext::createUniformBuffer(
    IUniformBuffer::CreateInfo createInfo)
{
    return std::make_shared<UniformBuffer>(*this, std::move(createInfo));
}

void GraphicsContext::waitIdle()
{
    vkDeviceWaitIdle(device());
}

VkDescriptorSetLayout GraphicsContext::descriptorSetLayout(uint32_t id) const
{
    return m_layouts.at(id);
}

Multisampling GraphicsContext::maxSampleCount() const
{
    const auto physicalDeviceInfo = this->physicalDeviceInfo();
    VkSampleCountFlags counts = physicalDeviceInfo.properties.limits.framebufferColorSampleCounts &
        physicalDeviceInfo.properties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
    {
        return Multisampling::MSA_64X;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT)
    {
        return Multisampling::MSA_32X;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT)
    {
        return Multisampling::MSA_16X;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT)
    {
        return Multisampling::MSA_8X;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT)
    {
        return Multisampling::MSA_4X;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT)
    {
        return Multisampling::MSA_2X;
    }

    return Multisampling::MSA_1X;
}

}    //  namespace renderer::vk
