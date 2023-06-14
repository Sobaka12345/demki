#include "graphical_application.hpp"

#include "image.hpp"
#include "vertex.hpp"

#include <limits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>

using namespace vk;

namespace
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cout << "Warning: " <<
        pCallbackData->messageIdNumber << ": " << pCallbackData->pMessageIdName << ":" <<  pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (GraphicalApplication::s_enableValidationLayers)
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

#ifdef NDEBUG
    const bool GraphicalApplication::s_enableValidationLayers = false;
#else
    const bool GraphicalApplication::s_enableValidationLayers = true;
#endif

const std::vector<const char*> GraphicalApplication::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
    //"VK_LAYER_RENDERDOC",
    //"VK_LAYER_LUNARG_api_dump",
    //"VK_LAYER_MESA_overlay",
    //"VK_LAYER_INTEL_nullhw",
    //"VK_LAYER_MESA_device_select",
    //"VK_LAYER_LUNARG_monitor"
};

GraphicalApplication::GraphicalApplication()
    : m_windowWidth(640)
    , m_windowHeight(480)
    , m_currentFrame(0)
    , m_maxFramesInFlight(2)
    , m_framebufferResized(false)
    , m_windowIconified(false)
{
}

GraphicalApplication::~GraphicalApplication()
{
    cleanupSwapChain();

    vkDestroyRenderPass(*m_device, m_vkRenderPass, nullptr);
    for (size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        vkDestroyFence(*m_device, m_vkInFlightFences[i], nullptr);
        vkDestroySemaphore(*m_device, m_vkImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(*m_device, m_vkRenderFinishedSemaphores[i], nullptr);
    }

    vkDestroyCommandPool(*m_device, m_vkCommandPool, nullptr);
    m_device.reset();
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
    auto start = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(m_window))
    {
        auto end = std::chrono::steady_clock::now();
        update(
            std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start).count()
        );
        start = end;

        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(*m_device);
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

void GraphicalApplication::initBase()
{
    // Basic
    createInstance();
    setupDebugMessenger();
    createWindowSurface();
    createLogicalDevice();
    createSyncObjects();
    createCommandPool();

    //// Swap Chain
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();

    createCommandBuffers();
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
        ASSERT(utils::requiredValidationLayerSupported(GraphicalApplication::s_validationLayers),
            "required validation layers are absent");

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_validationLayers.size());
        createInfo.ppEnabledLayerNames = s_validationLayers.data();
        createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo); // we need this to debug vk(Create|Destroy)Instance
    } else
    {
        createInfo.pNext = nullptr;
        createInfo.enabledLayerCount = 0;
    }

    const auto extensions = getRequiredExtensions();
    ASSERT(utils::requiredExtensionsSupported(extensions), "required extensions are absent");

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;

    ASSERT(vkCreateInstance(&createInfo, nullptr, &m_vkInstance) == VK_SUCCESS,
        "failed to create instance ;c");
}

void GraphicalApplication::setupDebugMessenger()
{
    if (!s_enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);
    ASSERT(createDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_vkDebugMessenger) == VK_SUCCESS,
        "failed to set up debug messenger!");
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

void GraphicalApplication::createLogicalDevice()
{
    m_device = std::make_unique<Device>(m_vkInstance, m_vkSurface);

    m_vkPresentQueue = m_device->queue(Device::GRAPHICS, 0);
    m_vkGraphicsQueue = m_device->queue(Device::PRESENT, 0);
}

void GraphicalApplication::createSwapChain() {
    const auto swapChainSupportDetails = utils::swapChainSupportDetails(m_device->physicalDevice(), m_vkSurface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities);
    uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
    if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount)
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    std::span<const uint32_t> queueFamilyIndices = {};
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->queueFamilies()[vk::Device::GRAPHICS] !=
        m_device->queueFamilies()[vk::Device::PRESENT])
    {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = {
            m_device->queueFamilies().begin(), m_device->queueFamilies().end()
        };
    }

    const auto createInfo = create::swapChainCreateInfoKHR(
        m_vkSurface, imageCount,
        surfaceFormat.format, surfaceFormat.colorSpace, extent, 1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        sharingMode, queueFamilyIndices,
        swapChainSupportDetails.capabilities.currentTransform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE);

    ASSERT(vkCreateSwapchainKHR(*m_device, &createInfo, nullptr, &m_vkSwapChain) == VK_SUCCESS,
        "failed to create swap chain!");

    vkGetSwapchainImagesKHR(*m_device, m_vkSwapChain, &imageCount, nullptr);
    m_vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*m_device, m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

    m_vkSwapChainImageFormat = surfaceFormat.format;
    m_vkSwapChainExtent = VkExtent3D{extent.width, extent.height, 1};
}

void GraphicalApplication::recreateSwapChain()
{
    while (m_windowIconified)
    {
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(*m_device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void GraphicalApplication::cleanupSwapChain()
{
    vkDestroyImageView(*m_device, m_depthImageView, nullptr);
    vkDestroyImage(*m_device, m_depthImage, nullptr);
    vkFreeMemory(*m_device, m_depthMemory, nullptr);

    for (auto framebuffer : m_vkSwapChainFramebuffers)
    {
        vkDestroyFramebuffer(*m_device, framebuffer, nullptr);
    }
    for (auto imageView : m_vkSwapChainImageViews)
    {
        vkDestroyImageView(*m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(*m_device, m_vkSwapChain, nullptr);
}

void GraphicalApplication::createWindowSurface()
{
    ASSERT(glfwCreateWindowSurface(m_vkInstance, m_window, nullptr, &m_vkSurface) == VK_SUCCESS,
        "failed to create window surface!");
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

    ASSERT(vkCreateRenderPass(*m_device, &renderPassInfo, nullptr, &m_vkRenderPass) == VK_SUCCESS,
        "failed to create render pass!");
}

VkPipeline GraphicalApplication::defaultGraphicsPipeline(VkDevice device,
    VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
    VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
    const std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
        create::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader),
        create::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader)
    };

    static constexpr std::array<VkDynamicState, 2> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    constexpr VkPipelineDynamicStateCreateInfo dynamicState =
        create::pipelineDynamicStateCreateInfo(dynamicStates);

    VkPipelineViewportStateCreateInfo viewportState =
        create::pipelineViewportStateCreateInfo(std::array{VkViewport{}}, std::array{VkRect2D{}});

    static constexpr auto bindingDescriptions = Vertex3DColored::getBindingDescription();
    static constexpr auto attributeDescriptions = Vertex3DColored::getAttributeDescriptions();

    constexpr VkPipelineVertexInputStateCreateInfo vertexInputInfo =
        create::pipelineVertexInputStateCreateInfo(bindingDescriptions, attributeDescriptions);

    constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly =
        create::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    constexpr VkPipelineRasterizationStateCreateInfo rasterizer = create::pipelineRasterizationStateCreateInfo(
        VK_FALSE, VK_FALSE,
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_CLOCKWISE,
        VK_FALSE
    );

    constexpr VkPipelineMultisampleStateCreateInfo multisampling =
        create::pipelineMultisampleStateCreateInfo(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

    constexpr VkPipelineDepthStencilStateCreateInfo depthStencil = create::pipelineDepthStencilStateCreateInfo(
        VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE
    );

    static constexpr std::array colorBlendAttachments = {
        create::pipelineColorBlendAttachmentState(
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        )
    };

    VkPipelineColorBlendStateCreateInfo colorBlending =
        create::pipelineColorBlendStateCreateInfo(colorBlendAttachments);

    const VkGraphicsPipelineCreateInfo pipelineInfo =
        create::graphicsPipelineCreateInfo(
            pipelineLayout, renderPass, 0, shaderStages,
            &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
            &colorBlending, &dynamicState, &depthStencil);

    VkPipeline result;
    ASSERT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &result) == VK_SUCCESS,
        "failed to create graphics pipeline!");

    return result;
}


void GraphicalApplication::createFramebuffers()
{
    VkFormat depthFormat = findDepthFormat();

//    Image depthimagef(*m_device,
//        create::imageCreateInfo(VK_IMAGE_TYPE_2D, m_vkSwapChainExtent, 1, 1,
//        depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED,
//        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT));
//    createImage(m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, depthFormat,
//        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthMemory);
//    const auto depthImageView = createImageView(depthImagef, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    createImage(m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthMemory);
    m_depthImageView = createImageView(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    m_vkSwapChainFramebuffers.resize(m_vkSwapChainImageViews.size());
    for (size_t i = 0; i < m_vkSwapChainImageViews.size(); i++)
    {
        const std::array<VkImageView, 2> attachments = {
            m_vkSwapChainImageViews[i],
            m_depthImageView
        };

        const VkFramebufferCreateInfo framebufferInfo = create::frameBufferCreateInfo(
            m_vkRenderPass, attachments,
            m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 1);

        ASSERT(vkCreateFramebuffer(*m_device, &framebufferInfo, nullptr, &m_vkSwapChainFramebuffers[i]) == VK_SUCCESS,
            "failed to create framebuffer!");
    }
}

void GraphicalApplication::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_device->queueFamilies()[Device::GRAPHICS];

    ASSERT(vkCreateCommandPool(*m_device, &poolInfo, nullptr, &m_vkCommandPool) == VK_SUCCESS,
        "failed to create command pool!");
}

VkFormat GraphicalApplication::findSupportedFormat(const std::vector<VkFormat>& candidates,
    VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice(), format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ASSERT(false, "failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
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
    ASSERT(vkCreateImageView(*m_device, &viewInfo, nullptr, &imageView) == VK_SUCCESS,
        "failed to create texture image view!");

    return imageView;
}

void GraphicalApplication::createImage(uint32_t width, uint32_t height,
    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo =
        create::imageCreateInfo(VK_IMAGE_TYPE_2D, create::extent3D(width, height, 1),
        1, 1, format, tiling, VK_IMAGE_LAYOUT_UNDEFINED,
        usage, VK_SAMPLE_COUNT_1_BIT, VK_SHARING_MODE_EXCLUSIVE);

    ASSERT(vkCreateImage(*m_device, &imageInfo, nullptr, &image) == VK_SUCCESS,
        "failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*m_device, image, &memRequirements);

    const auto allocInfo = create::memoryAllocateInfo(memRequirements.size,
        utils::findMemoryType(m_device->physicalDevice(), memRequirements.memoryTypeBits, properties));

    ASSERT(vkAllocateMemory(*m_device, &allocInfo, nullptr, &imageMemory) == VK_SUCCESS,
        "failed to allocate image memory!");

    ASSERT(vkBindImageMemory(*m_device, image, imageMemory, 0) == VK_SUCCESS);
}

void GraphicalApplication::createCommandBuffers()
{
    m_vkCommandBuffers.resize(m_maxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = m_vkCommandBuffers.size();

    ASSERT(vkAllocateCommandBuffers(*m_device, &allocInfo, m_vkCommandBuffers.data()) == VK_SUCCESS,
        "failed to allocate command buffers!");
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
        ASSERT(vkCreateFence(*m_device, &fenceInfo, nullptr, &m_vkInFlightFences[i]) == VK_SUCCESS,
            "failed to create fence");
        ASSERT(vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphores[i]) == VK_SUCCESS,
            "failed to create semaphore");
        ASSERT(vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphores[i]) == VK_SUCCESS,
            "failed to create semaphore");
    }
}

void GraphicalApplication::drawFrame()
{
    vkWaitForFences(*m_device, 1, &m_vkInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(*m_device, m_vkSwapChain, UINT64_MAX, m_vkImageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!");

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }

    vkResetFences(*m_device, 1, &m_vkInFlightFences[m_currentFrame]);
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

    ASSERT(vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, m_vkInFlightFences[m_currentFrame]) == VK_SUCCESS,
        "failed to submit draw command buffer!");

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
        ASSERT(false, "failed to present swap chain image!");
    }


    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

int GraphicalApplication::exec()
{
    initWindow();
    initBase();
    initApplication();
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
