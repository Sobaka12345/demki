#include "GraphicalApplication.hpp"

#include <limits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>

#include "Creators.hpp"
#include "Utils.hpp"

using namespace vk;

namespace
{

template<typename LayoutType>
struct UBObject
{
    LayoutType layout;
    static constexpr uint32_t size = sizeof(LayoutType);
};

struct UBOViewProj
{
    glm::mat4 view;
    glm::mat4 proj;
} s_viewProj;

struct UBOModel
{
    glm::mat4 model;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

static const std::vector<Vertex> s_kekVertices = {
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}, //0
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, //1
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}, //2
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, //3
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //4
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //5
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //6
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}  //7
};

static const std::vector<uint16_t> s_kekIndices = {
        //Top
        7, 6, 2,
        2, 3, 7,
        //Bottom
        0, 4, 5,
        5, 1, 0,
        //Left
        0, 2, 6,
        6, 4, 0,
        //Right
        7, 3, 1,
        1, 5, 7,
        //Front
        3, 2, 0,
        0, 1, 3,
        //Back
        4, 6, 7,
        7, 5, 4
};

static constexpr auto s_objectCount = 50;

#ifdef NDEBUG
    const bool s_enableValidationLayers = false;
#else
    const bool s_enableValidationLayers = false;
#endif

#if defined(VK_USE_PLATFORM_XCB_KHR)
    using VkPlatformSurfaceCreateInfo = VkXcbSurfaceCreateInfoKHR;
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    using VkPlatformSurfaceCreateInfo = VkWin32SurfaceCreateInfoKHR;
#endif

VkPlatformSurfaceCreateInfo createVkPlatformSurfaceCreateInfo(GLFWwindow* window)
{
#if defined(VK_USE_PLATFORM_XCB_KHR)
    VkXcbSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.window = glfwGetX11Window(window);
    //createInfo.connection = GetModuleHandle(nullptr);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(window);
    createInfo.hinstance = GetModuleHandle(nullptr);
#endif

    return createInfo;
}

VkResult vkCreatePlatformSurface(
    VkInstance                         instance,
    const VkPlatformSurfaceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*       pAllocator,
    VkSurfaceKHR*                      pSurface)
{
#if defined(VK_USE_PLATFORM_XCB_KHR)
    return vkCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    return vkCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
#endif
}

const std::vector<const char*> s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_LUNARG_api_dump",
    "VK_LAYER_INTEL_nullhw",
    "VK_LAYER_MESA_device_select",
    "VK_LAYER_LUNARG_monitor"
};

const std::vector<const char*> s_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cout << "Warning: " << pCallbackData->messageIdNumber << ": " << pCallbackData->pMessageIdName << ":" <<  pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (s_enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

}

GraphicalApplication& GraphicalApplication::instance()
{
    static GraphicalApplication instance;
    return instance;
}

GraphicalApplication::GraphicalApplication()
    : m_windowWidth(640)
    , m_windowHeight(480)
    , m_currentFrame(0)
    , m_maxFramesInFlight(2)
    , m_framebufferResized(false)
    , m_windowIconified(false)
    , m_vkPhysicalDevice(VK_NULL_HANDLE)
{
}

GraphicalApplication::~GraphicalApplication()
{
    cleanupSwapChain();
    for (size_t i = 0; i <  m_uniformBuffers.size(); ++i)
    {
        vkDestroyBuffer(m_vkLogicalDevice, m_uniformBuffers[i].m_viewProjectionBuffer.buffer, nullptr);
        vkFreeMemory(m_vkLogicalDevice, m_uniformBuffers[i].m_viewProjectionBuffer.memory, nullptr);
        vkDestroyBuffer(m_vkLogicalDevice, m_uniformBuffers[i].m_modelBufffer.buffer, nullptr);
        vkFreeMemory(m_vkLogicalDevice, m_uniformBuffers[i].m_modelBufffer.memory, nullptr);
        std::free(m_modelBuffers[i]);
    }
    vkDestroyDescriptorPool(m_vkLogicalDevice, m_vkDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_vkLogicalDevice, m_vkDescriptorSetLayout, nullptr);
    vkDestroyBuffer(m_vkLogicalDevice, m_vkIndexBuffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, m_vkIndexBufferMemory, nullptr);
    vkDestroyBuffer(m_vkLogicalDevice, m_vkVertexBuffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, m_vkVertexBufferMemory, nullptr);
    vkDestroyPipeline(m_vkLogicalDevice, m_vkPipeline, nullptr);
    vkDestroyPipelineLayout(m_vkLogicalDevice, m_vkPipelineLayout, nullptr);

    vkDestroyRenderPass(m_vkLogicalDevice, m_vkRenderPass, nullptr);
    for (size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        vkDestroyFence(m_vkLogicalDevice, m_vkInFlightFences[i], nullptr);
        vkDestroySemaphore(m_vkLogicalDevice, m_vkImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_vkLogicalDevice, m_vkRenderFinishedSemaphores[i], nullptr);
    }

    vkDestroyCommandPool(m_vkLogicalDevice, m_vkCommandPool, nullptr);
    vkDestroyDevice(m_vkLogicalDevice, nullptr);
    if (s_enableValidationLayers)
    {
        destroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
    vkDestroyInstance(m_vkInstance, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

int GraphicalApplication::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_vkLogicalDevice);
    return 0;
}

void GraphicalApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<GraphicalApplication*>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
}

void GraphicalApplication::windowIconifyCallback(GLFWwindow* window, int flag)
{
    auto app = reinterpret_cast<GraphicalApplication*>(glfwGetWindowUserPointer(window));
    app->m_windowIconified = flag;
}

void GraphicalApplication::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetWindowIconifyCallback(m_window, windowIconifyCallback);
}

void GraphicalApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createWindowSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createCommandPool();
    createDepthResources();
    createFramebuffers();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void GraphicalApplication::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = m_appName.c_str();
    appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    appInfo.pEngineName = "Testo";
    appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (s_enableValidationLayers)
    {
        if(!utils::requiredValidationLayerSupported(s_validationLayers))
        {
            throw std::runtime_error("required validation layers are absent");
        }
        else
        {
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
            createInfo.ppEnabledLayerNames = s_validationLayers.data();
            createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo); // we need this to debug vk(Create|Destroy)Instance
        }
    } else
    {
        createInfo.pNext = nullptr;
        createInfo.enabledLayerCount = 0;
    }

    const auto extensions = getRequiredExtensions();
    if (!utils::requiredExtensionsSupported(extensions))
    {
        throw std::runtime_error("required extensions are absent");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance ;c");
    }

}

void GraphicalApplication::setupDebugMessenger()
{
    if (!s_enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);
    if (createDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void GraphicalApplication::pickPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, nullptr);

    if (!physicalDeviceCount)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, physicalDevices.data());
    if (auto iter = std::find_if(physicalDevices.begin(), physicalDevices.end(),
            std::bind(std::mem_fn(&GraphicalApplication::isDeviceSuitable), this, std::placeholders::_1));
        iter == physicalDevices.end())
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    else
    {
        m_vkPhysicalDevice = *iter;
    }
}

bool GraphicalApplication::isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    m_vkPhysicalDeviceLimits = deviceProperties.limits;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    const bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return findQueueFamilies(device).isComplete()
           && extensionsSupported
           && swapChainAdequate;
}

bool GraphicalApplication::checkDeviceExtensionSupport(VkPhysicalDevice device) {
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

QueueFamilyIndices GraphicalApplication::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices result;
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
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_vkSurface, &presentSupport);
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

VkSurfaceFormatKHR GraphicalApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR GraphicalApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicalApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

SwapChainSupportDetails GraphicalApplication::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_vkSurface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void GraphicalApplication::createLogicalDevice()
{
    m_queueFamilyIndices = findQueueFamilies(m_vkPhysicalDevice);

    VkPhysicalDeviceFeatures deviceFeatures{};

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto& queueFamily : m_queueFamilyIndices.uniqueQueueFamilies())
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(s_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = s_deviceExtensions.data();
    if (s_enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
        createInfo.ppEnabledLayerNames = s_validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_vkPhysicalDevice, &createInfo, nullptr, &m_vkLogicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_vkLogicalDevice, m_queueFamilyIndices.presentFamily.value(), 0, &m_vkPresentQueue);
    vkGetDeviceQueue(m_vkLogicalDevice, m_queueFamilyIndices.graphicsFamily.value(), 0, &m_vkGraphicsQueue);
}

void GraphicalApplication::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_vkPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_vkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(m_vkPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_vkLogicalDevice, &createInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_vkLogicalDevice, m_vkSwapChain, &imageCount, nullptr);
    m_vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_vkLogicalDevice, m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

    m_vkSwapChainImageFormat = surfaceFormat.format;
    m_vkSwapChainExtent = extent;
}

void GraphicalApplication::recreateSwapChain()
{
    while (m_windowIconified)
    {
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_vkLogicalDevice);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createDepthResources();
    createFramebuffers();
}

void GraphicalApplication::cleanupSwapChain()
{
    for (auto framebuffer : m_vkSwapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_vkLogicalDevice, framebuffer, nullptr);
    }
    for (auto imageView : m_vkSwapChainImageViews)
    {
        vkDestroyImageView(m_vkLogicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_vkLogicalDevice, m_vkSwapChain, nullptr);
}

void GraphicalApplication::createWindowSurface()
{
    if (glfwCreateWindowSurface(m_vkInstance, m_window, nullptr, &m_vkSurface) != VK_SUCCESS)
    {
       throw std::runtime_error("failed to create window surface!");
    }
}

void GraphicalApplication::createImageViews()
{
    m_vkSwapChainImageViews.resize(m_vkSwapChainImages.size());

    for (size_t i = 0; i < m_vkSwapChainImages.size(); ++i)
    {
        m_vkSwapChainImageViews[i] = createImageView(m_vkSwapChainImages[i],
            m_vkSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void GraphicalApplication::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_vkLogicalDevice, &renderPassInfo, nullptr, &m_vkRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VkDescriptorSetLayoutBinding createSetLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags shaderStageFlags,
    uint32_t binding, uint32_t descriptorCount = 1)
{
    VkDescriptorSetLayoutBinding setLayoutBinding{};
    setLayoutBinding.binding = binding;
    setLayoutBinding.descriptorCount = descriptorCount;
    setLayoutBinding.descriptorType = descriptorType;
    setLayoutBinding.pImmutableSamplers = nullptr;
    setLayoutBinding.stageFlags = shaderStageFlags;

    return setLayoutBinding;
}

void GraphicalApplication::createDescriptorSetLayout()
{
    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        createSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 0),
        createSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_vkLogicalDevice, &layoutInfo, nullptr, &m_vkDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void GraphicalApplication::createGraphicsPipeline()
{
    auto vertShaderCode = utils::fs::readFile("./shaders/shader.vert.spv");
    auto fragShaderCode = utils::fs::readFile("./shaders/shader.frag.spv");
    VkShaderModule vertShaderModule = create::shaderModule(m_vkLogicalDevice, vertShaderCode);
    VkShaderModule fragShaderModule = create::shaderModule(m_vkLogicalDevice, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    const auto bindingDescription = Vertex::getBindingDescription();
    const auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

//    VkViewport viewport{};
//    viewport.x = 0.0f;
//    viewport.y = 0.0f;
//    viewport.width = (float) m_vkSwapChainExtent.width;
//    viewport.height = (float) m_vkSwapChainExtent.height;
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;

//    VkRect2D scissor{};
//    scissor.offset = {0, 0};
//    scissor.extent = m_vkSwapChainExtent;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &m_vkDescriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(m_vkLogicalDevice, &pipelineLayoutInfo, nullptr, &m_vkPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_vkPipelineLayout;
    pipelineInfo.renderPass = m_vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(m_vkLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_vkPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_vkLogicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_vkLogicalDevice, vertShaderModule, nullptr);
}

void GraphicalApplication::createFramebuffers()
{
    m_vkSwapChainFramebuffers.resize(m_vkSwapChainImageViews.size());
    for (size_t i = 0; i < m_vkSwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
            m_vkSwapChainImageViews[i],
            m_vkDepthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_vkRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_vkSwapChainExtent.width;
        framebufferInfo.height = m_vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_vkLogicalDevice, &framebufferInfo, nullptr, &m_vkSwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void GraphicalApplication::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_vkPhysicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_vkLogicalDevice, &poolInfo, nullptr, &m_vkCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

VkFormat GraphicalApplication::findSupportedFormat(const std::vector<VkFormat>& candidates,
    VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat GraphicalApplication::findDepthFormat()
{
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool GraphicalApplication::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkImageView GraphicalApplication::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    const auto viewInfo = create::imageViewCreateInfo(image, VK_IMAGE_VIEW_TYPE_2D, format,
        create::imageSubresourceRange(aspectFlags, 0, 1, 0, 1));

    VkImageView imageView;
    if (vkCreateImageView(m_vkLogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void GraphicalApplication::createImage(uint32_t width, uint32_t height,
    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo =
        create::imageCreateInfo(VK_IMAGE_TYPE_2D, create::extent3D(width, height, 1),
        1, 1, format, tiling, VK_IMAGE_LAYOUT_UNDEFINED,
        usage, VK_SAMPLE_COUNT_1_BIT, VK_SHARING_MODE_EXCLUSIVE);

    if (vkCreateImage(m_vkLogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_vkLogicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = create::memoryAllocateInfo(memRequirements.size,
        utils::findMemoryType(m_vkPhysicalDevice, memRequirements.memoryTypeBits, properties));

    if (vkAllocateMemory(m_vkLogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_vkLogicalDevice, image, imageMemory, 0);
}

void GraphicalApplication::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();
    createImage(m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkDepthImage, m_vkDepthImageMemory);
    m_vkDepthImageView = createImageView(m_vkDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void GraphicalApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_vkCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_vkLogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_vkGraphicsQueue);

    vkFreeCommandBuffers(m_vkLogicalDevice, m_vkCommandPool, 1, &commandBuffer);
}

void GraphicalApplication::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(s_kekVertices[0]) * s_kekVertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_vkLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, s_kekVertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_vkLogicalDevice, stagingBufferMemory);

    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkVertexBuffer, m_vkVertexBufferMemory);

    copyBuffer(stagingBuffer, m_vkVertexBuffer, bufferSize);

    vkDestroyBuffer(m_vkLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, stagingBufferMemory, nullptr);
}

void GraphicalApplication::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(s_kekIndices[0]) * s_kekIndices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_vkLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, s_kekIndices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_vkLogicalDevice, stagingBufferMemory);

    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkIndexBuffer, m_vkIndexBufferMemory);

    copyBuffer(stagingBuffer, m_vkIndexBuffer, bufferSize);

    vkDestroyBuffer(m_vkLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, stagingBufferMemory, nullptr);
}

void GraphicalApplication::createUniformBuffers()
{
    m_uniformBuffers.resize(m_maxFramesInFlight);
    m_modelBuffers.resize(m_maxFramesInFlight);

    const auto minAlignment = m_vkPhysicalDeviceLimits.minUniformBufferOffsetAlignment;
    m_dynamicAlignment = sizeof(glm::mat4x4);
    if (minAlignment > 0)
    {
        m_dynamicAlignment = (m_dynamicAlignment + minAlignment - 1) & ~(minAlignment - 1);
    }

    VkDeviceSize modelSize = m_dynamicAlignment* s_objectCount;
    VkDeviceSize viewProjSize = sizeof(UBOViewProj);

    for (size_t i = 0; i < m_maxFramesInFlight; i++)
    {
        m_modelBuffers[i] = reinterpret_cast<glm::mat4x4*>(std::aligned_alloc(m_dynamicAlignment, s_objectCount * m_dynamicAlignment));

        create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, modelSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m_uniformBuffers[i].m_modelBufffer.buffer, m_uniformBuffers[i].m_modelBufffer.memory);
        vkMapMemory(m_vkLogicalDevice, m_uniformBuffers[i].m_modelBufffer.memory, 0,
            modelSize, 0, &m_uniformBuffers[i].m_modelBufffer.mapped);
        m_uniformBuffers[i].m_modelBufffer.descriptor =
                create::descriptorBufferInfo(m_uniformBuffers[i].m_modelBufffer.buffer, 0, m_dynamicAlignment);

        create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, viewProjSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_uniformBuffers[i].m_viewProjectionBuffer.buffer, m_uniformBuffers[i].m_viewProjectionBuffer.memory);
        vkMapMemory(m_vkLogicalDevice, m_uniformBuffers[i].m_viewProjectionBuffer.memory, 0,
            viewProjSize, 0, &m_uniformBuffers[i].m_viewProjectionBuffer.mapped);
        m_uniformBuffers[i].m_viewProjectionBuffer.descriptor =
                create::descriptorBufferInfo(m_uniformBuffers[i].m_viewProjectionBuffer.buffer, 0, sizeof(UBOViewProj));

        updateUniformBuffer(i);
        updateDynUniformBuffer(i);
    }

}

VkDescriptorPoolSize createDescriptorPoolSize(
	VkDescriptorType type, uint32_t descriptorCount)
{
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = type;
    descriptorPoolSize.descriptorCount = descriptorCount;

    return descriptorPoolSize;
}

void GraphicalApplication::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{
        createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2),
        createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_maxFramesInFlight * 2;

    if (vkCreateDescriptorPool(m_vkLogicalDevice, &poolInfo, nullptr, &m_vkDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void GraphicalApplication::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(m_maxFramesInFlight, m_vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo =
            create::descriptorSetAllocateInfo(m_vkDescriptorPool, layouts.data(), static_cast<uint32_t>(m_maxFramesInFlight));

    m_vkDescriptorSets.resize(m_maxFramesInFlight);
    if (vkAllocateDescriptorSets(m_vkLogicalDevice, &allocInfo, m_vkDescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < m_maxFramesInFlight; i++)
    {
        std::array<VkWriteDescriptorSet, 2> writes = {
            create::writeDescriptorSet(m_vkDescriptorSets[i], 0,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &m_uniformBuffers[i].m_modelBufffer.descriptor),
            create::writeDescriptorSet(m_vkDescriptorSets[i], 1,
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &m_uniformBuffers[i].m_viewProjectionBuffer.descriptor),
        };
        vkUpdateDescriptorSets(m_vkLogicalDevice, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }
}

void GraphicalApplication::createCommandBuffers()
{
    m_vkCommandBuffers.resize(m_maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = m_vkCommandBuffers.size();

    if (vkAllocateCommandBuffers(m_vkLogicalDevice, &allocInfo, m_vkCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void GraphicalApplication::createSyncObjects()
{
    m_vkInFlightFences.resize(m_maxFramesInFlight);
    m_vkImageAvailableSemaphores.resize(m_maxFramesInFlight);
    m_vkRenderFinishedSemaphores.resize(m_maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    for(size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        if (vkCreateFence(m_vkLogicalDevice, &fenceInfo, nullptr, &m_vkInFlightFences[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_vkLogicalDevice, &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_vkLogicalDevice, &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

void GraphicalApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(m_vkCommandBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_vkRenderPass;
    renderPassInfo.framebuffer = m_vkSwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vkSwapChainExtent.width);
    viewport.height = static_cast<float>(m_vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_vkSwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {m_vkVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_vkIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
    for(uint32_t i = 0; i < s_objectCount; ++i)
    {
        uint32_t offset = i * m_dynamicAlignment;
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vkPipelineLayout, 0, 1, &m_vkDescriptorSets[m_currentFrame], 1, &offset);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(s_kekIndices.size()), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void GraphicalApplication::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

     s_viewProj.view = glm::lookAt(
         glm::vec3(3.0f, 3.0f, 3.0f),
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(0.0f, 0.0f, 1.0f));
     s_viewProj.proj = glm::perspective(
         glm::radians(90.0f),
         m_vkSwapChainExtent.width / static_cast<float>(m_vkSwapChainExtent.height),
         0.1f,
         20.0f);
     s_viewProj.proj[1][1] *= -1;
     memcpy(m_uniformBuffers[currentImage].m_viewProjectionBuffer.mapped, &s_viewProj, sizeof(UBOViewProj));
}

void GraphicalApplication::updateDynUniformBuffer(uint32_t currentImage)
{
    for(uint32_t i = 0; i < s_objectCount; ++i)
    {
        glm::mat4x4* model = reinterpret_cast<glm::mat4x4*>(reinterpret_cast<uint64_t>(m_modelBuffers[currentImage]) + i * m_dynamicAlignment);

        *model = glm::translate(glm::mat4x4(1.0f), -glm::vec3(0.0f, i * 1.0f, 0.0f));
        *model = glm::rotate(
            *model,
            glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
    }
    memcpy(m_uniformBuffers[currentImage].m_modelBufffer.mapped, m_modelBuffers[currentImage], m_dynamicAlignment * s_objectCount);
    VkMappedMemoryRange memoryRange{};
    memoryRange.memory = m_uniformBuffers[currentImage].m_modelBufffer.memory;
    memoryRange.size = m_dynamicAlignment * s_objectCount;
    vkFlushMappedMemoryRanges(m_vkLogicalDevice, 1, &memoryRange);
}

void GraphicalApplication::drawFrame()
{
    vkWaitForFences(m_vkLogicalDevice, 1, &m_vkInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(m_vkLogicalDevice, m_vkSwapChain, UINT64_MAX, m_vkImageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(m_vkLogicalDevice, 1, &m_vkInFlightFences[m_currentFrame]);
    vkResetCommandBuffer(m_vkCommandBuffers[m_currentFrame], 0);
    recordCommandBuffer(m_vkCommandBuffers[m_currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_vkImageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_vkCommandBuffers[m_currentFrame];
    VkSemaphore signalSemaphores[] = {m_vkRenderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, m_vkInFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    updateDynUniformBuffer(m_currentFrame);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_vkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    result = vkQueuePresentKHR(m_vkPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized || m_windowIconified)
    {
        m_framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

int GraphicalApplication::exec()
{
    initWindow();
    initVulkan();
    return mainLoop();
}

void GraphicalApplication::setWindowSize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
}

void GraphicalApplication::setApplicationName(std::string applicationName)
{
    m_appName = std::move(applicationName);
}
