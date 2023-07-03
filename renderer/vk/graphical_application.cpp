#include "graphical_application.hpp"

#include "image.hpp"
#include "queue.hpp"
#include "resource_manager.hpp"

#include "vk/command_pool.hpp"
#include "vk/render_pass.hpp"

#include <limits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>

namespace vk {

BEGIN_DECLARE_VKSTRUCT(ApplicationInfo, VK_STRUCTURE_TYPE_APPLICATION_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(const char*, pApplicationName)
    VKSTRUCT_PROPERTY(uint32_t, applicationVersion)
    VKSTRUCT_PROPERTY(const char*, pEngineName)
    VKSTRUCT_PROPERTY(uint32_t, engineVersion)
    VKSTRUCT_PROPERTY(uint32_t, apiVersion)
END_DECLARE_VKSTRUCT();

BEGIN_DECLARE_VKSTRUCT(InstanceCreateInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkInstanceCreateFlags, flags)
    VKSTRUCT_PROPERTY(const VkApplicationInfo*, pApplicationInfo)
    VKSTRUCT_PROPERTY(uint32_t, enabledLayerCount)
    VKSTRUCT_PROPERTY(const char* const*, ppEnabledLayerNames)
    VKSTRUCT_PROPERTY(uint32_t, enabledExtensionCount)
    VKSTRUCT_PROPERTY(const char* const*, ppEnabledExtensionNames)
END_DECLARE_VKSTRUCT();

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
const bool GraphicalApplication::s_enableValidationLayers = false;
#else
const bool GraphicalApplication::s_enableValidationLayers = true;
#endif

std::vector<const char*> getRequiredExtensions()
{
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

const std::vector<const char*> GraphicalApplication::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
    //"VK_LAYER_LUNARG_api_dump",
    //"VK_LAYER_MESA_overlay",
    //"VK_LAYER_INTEL_nullhw",
    //"VK_LAYER_MESA_device_select",
    //"VK_LAYER_LUNARG_monitor"
};

GraphicalApplication::GraphicalApplication()
    : Handle(nullptr)
    , m_windowWidth(640)
    , m_windowHeight(480)
    , m_currentFrame(0)
    , m_maxFramesInFlight(2)
    , m_framebufferResized(false)
    , m_windowIconified(false)
{}

GraphicalApplication::~GraphicalApplication()
{
    cleanupSwapChain();

    m_renderPass.reset();

    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();

    m_resourceManager.reset();
    m_device.reset();
    m_debugMessenger.reset();

    vkDestroySurfaceKHR(handle(), m_vkSurface, nullptr);
    destroy(vkDestroyInstance, handle(), nullptr);

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
            std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start)
                .count());
        start = end;

        glfwPollEvents();
        drawFrame();
    }

    m_device->waitIdle();
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
    createInstance();

    if (s_enableValidationLayers)
    {
        m_debugMessenger = std::make_unique<DebugUtilsMessenger>(*this, s_debugMessengerCreateInfo);
    }

    createWindowSurface();

    m_device = std::make_unique<Device>(handle(), m_vkSurface);
    m_presentQueue = m_device->queue(PRESENT);
    m_graphicsQueue = m_device->queue(GRAPHICS);

    createSyncObjects();

    //  Swap Chain
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();

    m_commandBuffers = m_device->commandPool(GRAPHICS).lock()->allocateBuffers(m_maxFramesInFlight);
}

void GraphicalApplication::createInstance()
{
    auto appInfo =
        ApplicationInfo()
            .pApplicationName(m_appName.c_str())
            .applicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .pEngineName("DemkiEngine")
            .engineVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .apiVersion(VK_API_VERSION_1_3);

    auto createInfo = InstanceCreateInfo().pApplicationInfo(&appInfo);

    if (s_enableValidationLayers)
    {
        ASSERT(utils::requiredValidationLayerSupported(GraphicalApplication::s_validationLayers),
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
    ASSERT(utils::requiredExtensionsSupported(extensions), "required extensions are absent");

    createInfo.enabledExtensionCount(extensions.size()).ppEnabledExtensionNames(extensions.data());

    ASSERT(create(vkCreateInstance, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create instance ;c");
}

VkSurfaceFormatKHR GraphicalApplication::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
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

VkPresentModeKHR GraphicalApplication::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicalApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void GraphicalApplication::createSwapChain()
{
    const auto swapChainSupportDetails =
        utils::swapChainSupportDetails(m_device->physicalDevice(), m_vkSurface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities);
    uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
    if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupportDetails.capabilities.maxImageCount)
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    std::span<const uint32_t> queueFamilyIndices = {};
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->queueFamilies()[GRAPHICS] != m_device->queueFamilies()[PRESENT])
    {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = { m_device->queueFamilies().begin(), m_device->queueFamilies().end() };
    }

    const auto createInfo = create::swapChainCreateInfoKHR(m_vkSurface,
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        extent,
        1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        sharingMode,
        queueFamilyIndices,
        swapChainSupportDetails.capabilities.currentTransform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        presentMode,
        VK_TRUE);

    ASSERT(vkCreateSwapchainKHR(*m_device, &createInfo, nullptr, &m_vkSwapChain) == VK_SUCCESS,
        "failed to create swap chain!");

    vkGetSwapchainImagesKHR(*m_device, m_vkSwapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount, *m_device);
    vkGetSwapchainImagesKHR(*m_device, m_vkSwapChain, &imageCount, m_swapChainImages.handleData());

    m_vkSwapChainImageFormat = surfaceFormat.format;
    m_vkSwapChainExtent = VkExtent3D{ extent.width, extent.height, 1 };
}

void GraphicalApplication::recreateSwapChain()
{
    while (m_windowIconified)
    {
        glfwWaitEvents();
    }
    m_device->waitIdle();

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void GraphicalApplication::cleanupSwapChain()
{
    m_depthImageView.reset();
    m_depthImage.reset();

    m_swapChainFramebuffers.clear();
    m_swapChainImageViews.clear();
    m_swapChainImages.clear();
    vkDestroySwapchainKHR(*m_device, m_vkSwapChain, nullptr);
}

void GraphicalApplication::createWindowSurface()
{
    ASSERT(glfwCreateWindowSurface(handle(), m_window, nullptr, &m_vkSurface) == VK_SUCCESS,
        "failed to create window surface!");
}

void GraphicalApplication::createImageViews()
{
    for (size_t i = 0; i < m_swapChainImages.size(); ++i)
    {
        m_swapChainImageViews.emplace_back(*m_device,
            defaultImageView(m_swapChainImages[i], m_vkSwapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT));
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
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    m_renderPass = std::make_unique<RenderPass>(*m_device,
        RenderPassCreateInfo()
            .attachmentCount(attachments.size())
            .pAttachments(attachments.data())
            .subpassCount(1)
            .pSubpasses(&subpass)
            .dependencyCount(1)
            .pDependencies(&dependency));
}

GraphicsPipelineCreateInfo GraphicalApplication::defaultGraphicsPipeline()
{
    static constexpr std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR };

    static constexpr create::PipelineDynamicStateCreateInfo dynamicState =
        create::PipelineDynamicStateCreateInfo()
            .dynamicStateCount(dynamicStates.size())
            .pDynamicStates(dynamicStates.data());

    static constexpr create::PipelineViewportStateCreateInfo viewportState =
        create::PipelineViewportStateCreateInfo().viewportCount(1).scissorCount(1);

    static constexpr create::PipelineInputAssemblyStateCreateInfo inputAssembly =
        create::PipelineInputAssemblyStateCreateInfo()
            .topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .primitiveRestartEnable(VK_FALSE);

    static constexpr create::PipelineRasterizationStateCreateInfo rasterizer =
        create::PipelineRasterizationStateCreateInfo()
            .depthClampEnable(VK_FALSE)
            .rasterizerDiscardEnable(VK_FALSE)
            .polygonMode(VK_POLYGON_MODE_FILL)
            .cullMode(VK_CULL_MODE_BACK_BIT)
            .frontFace(VK_FRONT_FACE_CLOCKWISE)
            .depthBiasEnable(VK_FALSE)
            .lineWidth(1.0f)
            .depthBiasClamp(0.0f)
            .depthBiasConstantFactor(0.0f)
            .depthBiasSlopeFactor(0.0f);

    static constexpr create::PipelineMultisampleStateCreateInfo multisampling =
        create::PipelineMultisampleStateCreateInfo()
            .sampleShadingEnable(VK_FALSE)
            .rasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
            .minSampleShading(1.0f)
            .pSampleMask(nullptr)
            .alphaToCoverageEnable(VK_FALSE)
            .alphaToOneEnable(VK_FALSE);

    static constexpr VkPipelineDepthStencilStateCreateInfo depthStencil =
        create::PipelineDepthStencilStateCreateInfo()
            .depthTestEnable(VK_TRUE)
            .depthWriteEnable(VK_TRUE)
            .depthCompareOp(VK_COMPARE_OP_LESS)
            .depthBoundsTestEnable(VK_FALSE)
            .stencilTestEnable(VK_FALSE);

    static constexpr std::array colorBlendAttachments = {
        create::PipelineColorBlendAttachmentState()
            .colorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
            .colorBlendOp(VK_BLEND_OP_ADD)
            .srcColorBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstColorBlendFactor(VK_BLEND_FACTOR_ZERO)
            .alphaBlendOp(VK_BLEND_OP_ADD)
            .srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO)
    };

    static constexpr create::PipelineColorBlendStateCreateInfo colorBlending =
        create::PipelineColorBlendStateCreateInfo()
            .attachmentCount(colorBlendAttachments.size())
            .pAttachments(colorBlendAttachments.data())
            .logicOp(VK_LOGIC_OP_COPY)
            .blendConstants({ 0.0f, 0.0f, 0.0f, 0.0f });

    return GraphicsPipelineCreateInfo()
        .layout(VK_NULL_HANDLE)
        .renderPass(VK_NULL_HANDLE)
        .pStages(nullptr)
        .stageCount(0)
        .pVertexInputState(nullptr)
        .pInputAssemblyState(&inputAssembly)
        .pTessellationState(nullptr)
        .pViewportState(&viewportState)
        .pRasterizationState(&rasterizer)
        .pMultisampleState(&multisampling)
        .pColorBlendState(&colorBlending)
        .pDynamicState(&dynamicState)
        .pDepthStencilState(&depthStencil)
        .basePipelineHandle(VK_NULL_HANDLE)
        .basePipelineIndex(-1)
        .subpass(0)
        .flags(0)
        .pNext(nullptr);
}

void GraphicalApplication::createFramebuffers()
{
    const auto depthFormat = findDepthFormat();

    m_depthImage = std::make_unique<Image>(*m_device,
        ImageCreateInfo{}
            .imageType(VK_IMAGE_TYPE_2D)
            .extent(m_vkSwapChainExtent)
            .mipLevels(1)
            .arrayLayers(1)
            .format(depthFormat)
            .tiling(VK_IMAGE_TILING_OPTIMAL)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_depthImage->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_depthImageView = std::make_unique<ImageView>(*m_device,
        defaultImageView(*m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT));

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        const std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i],
            *m_depthImageView };

        const VkFramebufferCreateInfo framebufferInfo = create::frameBufferCreateInfo(*m_renderPass,
            attachments, m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 1);

        m_swapChainFramebuffers.emplace_back(*m_device, framebufferInfo);
    }
}

VkFormat GraphicalApplication::findSupportedFormat(
    const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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

VkFormat GraphicalApplication::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool GraphicalApplication::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

ResourceManager& GraphicalApplication::resources()
{
    return *m_resourceManager;
}

ImageViewCreateInfo GraphicalApplication::defaultImageView(
    VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    return ImageViewCreateInfo()
        .image(image)
        .viewType(VK_IMAGE_VIEW_TYPE_2D)
        .format(format)
        .subresourceRange(create::imageSubresourceRange(aspectFlags, 0, 1, 0, 1));
}

void GraphicalApplication::createSyncObjects()
{
    constexpr FenceCreateInfo fenceInfo = FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT);

    for (size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_inFlightFences.emplace_back(*m_device,
            FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT));
        m_imageAvailableSemaphores.emplace_back(*m_device, SemaphoreCreateInfo{});
        m_renderFinishedSemaphores.emplace_back(*m_device, SemaphoreCreateInfo{});
    }
}

void GraphicalApplication::drawFrame()
{
    vkWaitForFences(*m_device, 1, m_inFlightFences[m_currentFrame].handlePtr(), VK_TRUE,
        UINT64_MAX);
    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(*m_device, m_vkSwapChain, UINT64_MAX,
        m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swap chain image!");

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }

    vkResetFences(*m_device, 1, m_inFlightFences[m_currentFrame].handlePtr());

    const auto& commandBuffer = m_commandBuffers[m_currentFrame];
    commandBuffer.reset();

    ASSERT(commandBuffer.begin() == VK_SUCCESS, "failed to begin recording command buffer!");
    recordCommandBuffer(commandBuffer, m_swapChainFramebuffers[imageIndex]);
    ASSERT(commandBuffer.end() == VK_SUCCESS, "failed to record command buffer!");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_imageAvailableSemaphores[m_currentFrame].handlePtr();
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer.handlePtr();

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = m_renderFinishedSemaphores[m_currentFrame].handlePtr();

    ASSERT(m_graphicsQueue.lock()->submit(1, &submitInfo, m_inFlightFences[m_currentFrame]) ==
            VK_SUCCESS,
        "failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = m_renderFinishedSemaphores[m_currentFrame].handlePtr();

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_vkSwapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;    //  Optional
    result = m_presentQueue.lock()->presentKHR(&presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized ||
        m_windowIconified)
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
    m_resourceManager = std::make_unique<ResourceManager>(*m_device);
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

}    //  namespace vk
