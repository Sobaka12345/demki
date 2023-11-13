#include "graphics_context.hpp"

#include "resource_manager.hpp"

#include "handles/surface.hpp"

#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"

#include <render_context.hpp>
#include <window.hpp>

#include <cstring>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {

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
    handles::DebugUtilsMessengerCreateInfoEXT()
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

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (GraphicsContext::s_enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

GraphicsContext::GraphicsContext(const Window& window)
    : m_window(window)
{
    auto appInfo =
        handles::ApplicationInfo()
            .pApplicationName(window.name().c_str())
            .applicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .pEngineName("DemkiEngine")
            .engineVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .apiVersion(VK_API_VERSION_1_3);

    auto createInfo = handles::InstanceCreateInfo().pApplicationInfo(&appInfo);

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

    ASSERT(create(vkCreateInstance, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create instance ;c");

    if (s_enableValidationLayers)
    {
        m_debugMessenger =
            std::make_unique<handles::DebugUtilsMessenger>(*this, s_debugMessengerCreateInfo);
    }

    m_surface = std::make_unique<handles::Surface>(*this, m_window.glfwHandle());
    m_device = std::make_unique<handles::Device>(handle(), *m_surface);
    m_resourceManager = std::make_unique<ResourceManager>(*this);
}

GraphicsContext::~GraphicsContext()
{
    m_resourceManager.reset();
    m_device.reset();
    m_debugMessenger.reset();
    m_surface.reset();
}

VkFormat GraphicsContext::findSupportedFormat(const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice(), format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
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

VkFormat GraphicsContext::findDepthFormat() const
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool GraphicsContext::hasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

ResourceManager& GraphicsContext::resources()
{
    return *m_resourceManager;
}

const handles::Surface& GraphicsContext::surface() const
{
    return *m_surface;
}

const handles::Device& GraphicsContext::device() const
{
    return *m_device;
}

const Window& GraphicsContext::window() const
{
    return m_window;
}

std::shared_ptr<IPipeline> GraphicsContext::createPipeline(IPipeline::CreateInfo createInfo) const
{
    if (createInfo.type() == IPipeline::GRAPHICS)
        return std::make_shared<GraphicsPipeline>(*this, std::move(createInfo));
    else if (createInfo.type() == IPipeline::COMPUTE)
        return std::make_shared<ComputePipeline>(*this, std::move(createInfo));

    ASSERT(false, "not implemented");
    return nullptr;
}

std::shared_ptr<IRenderer> GraphicsContext::createRenderer(IRenderer::CreateInfo createInfo) const
{
    return std::make_shared<Renderer>(*this, std::move(createInfo));
}

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(
    ISwapchain::CreateInfo createInfo) const
{
    return std::make_shared<Swapchain>(*this, std::move(createInfo));
}

IResourceManager& GraphicsContext::resources() const
{
    return *m_resourceManager;
}

void GraphicsContext::waitIdle()
{
    m_device->waitIdle();
}

Multisampling GraphicsContext::maxSampleCount() const
{
    VkSampleCountFlags counts =
        m_device->physicalDeviceProperties().limits.framebufferColorSampleCounts &
        m_device->physicalDeviceProperties().limits.framebufferDepthSampleCounts;
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

}    //  namespace vk
