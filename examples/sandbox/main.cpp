// main.cpp
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

int main() {
    const uint32_t kWidth = 800;
    const uint32_t kHeight = 600;
    const int kMaxFramesInFlight = 2;

    auto check = [](VkResult result, const char* msg) {
        if (result != VK_SUCCESS) {
            throw std::runtime_error(msg);
        }
    };

    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Vulkan Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    VkInstance instance = VK_NULL_HANDLE;
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "None";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        uint32_t glfwExtCount = 0;
        const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        std::vector<const char*> instanceExts(glfwExts, glfwExts + glfwExtCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExts.size());
        createInfo.ppEnabledExtensionNames = instanceExts.data();

        check(vkCreateInstance(&createInfo, nullptr, &instance), "vkCreateInstance failed");
    }

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    check(glfwCreateWindowSurface(instance, window, nullptr, &surface),
          "glfwCreateWindowSurface failed");

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() const { return graphicsFamily && presentFamily; }
    };

    auto findQueueFamilies = [&](VkPhysicalDevice dev) {
        QueueFamilyIndices indices;
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        std::vector<VkQueueFamilyProperties> props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());

        for (uint32_t i = 0; i < count; ++i) {
            if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (indices.isComplete()) {
                break;
            }
        }
        return indices;
    };

    auto checkDeviceExtensions = [&](VkPhysicalDevice dev) {
        uint32_t count = 0;
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> exts(count);
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, exts.data());

        const char* required = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        for (const auto& e : exts) {
            if (std::strcmp(e.extensionName, required) == 0) {
                return true;
            }
        }
        return false;
    };

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        if (count == 0) {
            throw std::runtime_error("No Vulkan devices found");
        }
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());

        for (auto dev : devices) {
            QueueFamilyIndices indices = findQueueFamilies(dev);
            if (!indices.isComplete()) {
                continue;
            }
            if (!checkDeviceExtensions(dev)) {
                continue;
            }
            physicalDevice = dev;
            break;
        }
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("No suitable GPU found");
        }
    }

    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    QueueFamilyIndices qIndices = findQueueFamilies(physicalDevice);
    {
        float priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        std::array<uint32_t, 2> uniqueFamilies = {
            qIndices.graphicsFamily.value(),
            qIndices.presentFamily.value()
        };
        if (uniqueFamilies[0] == uniqueFamilies[1]) {
            VkDeviceQueueCreateInfo qinfo{};
            qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qinfo.queueFamilyIndex = uniqueFamilies[0];
            qinfo.queueCount = 1;
            qinfo.pQueuePriorities = &priority;
            queueInfos.push_back(qinfo);
        } else {
            for (uint32_t fam : uniqueFamilies) {
                VkDeviceQueueCreateInfo qinfo{};
                qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                qinfo.queueFamilyIndex = fam;
                qinfo.queueCount = 1;
                qinfo.pQueuePriorities = &priority;
                queueInfos.push_back(qinfo);
            }
        }

        const char* deviceExts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.enabledExtensionCount = 1;
        createInfo.ppEnabledExtensionNames = deviceExts;

        check(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
              "vkCreateDevice failed");

        vkGetDeviceQueue(device, qIndices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, qIndices.presentFamily.value(), 0, &presentQueue);
    }

    struct SwapchainSupport {
        VkSurfaceCapabilitiesKHR caps{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    auto querySwapchainSupport = [&](VkPhysicalDevice dev) {
        SwapchainSupport s{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &s.caps);

        uint32_t count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, nullptr);
        s.formats.resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, s.formats.data());

        vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, nullptr);
        s.presentModes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, s.presentModes.data());

        return s;
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent{};
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    {
        SwapchainSupport s = querySwapchainSupport(physicalDevice);

        VkSurfaceFormatKHR chosenFormat = s.formats[0];
        for (const auto& f : s.formats) {
            if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
                f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                chosenFormat = f;
                break;
            }
        }

        VkPresentModeKHR chosenPresent = VK_PRESENT_MODE_FIFO_KHR;
        for (const auto& pm : s.presentModes) {
            if (pm == VK_PRESENT_MODE_MAILBOX_KHR) {
                chosenPresent = pm;
                break;
            }
        }

        if (s.caps.currentExtent.width != UINT32_MAX) {
            swapchainExtent = s.caps.currentExtent;
        } else {
            swapchainExtent = { kWidth, kHeight };
            swapchainExtent.width = std::max(s.caps.minImageExtent.width,
                                            std::min(s.caps.maxImageExtent.width, swapchainExtent.width));
            swapchainExtent.height = std::max(s.caps.minImageExtent.height,
                                             std::min(s.caps.maxImageExtent.height, swapchainExtent.height));
        }

        uint32_t imageCount = s.caps.minImageCount + 1;
        if (s.caps.maxImageCount > 0 && imageCount > s.caps.maxImageCount) {
            imageCount = s.caps.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = chosenFormat.format;
        createInfo.imageColorSpace = chosenFormat.colorSpace;
        createInfo.imageExtent = swapchainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {
            qIndices.graphicsFamily.value(),
            qIndices.presentFamily.value()
        };
        if (qIndices.graphicsFamily != qIndices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = s.caps.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = chosenPresent;
        createInfo.clipped = VK_TRUE;

        check(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain),
              "vkCreateSwapchainKHR failed");

        swapchainFormat = chosenFormat.format;

        uint32_t swapCount = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &swapCount, nullptr);
        swapchainImages.resize(swapCount);
        vkGetSwapchainImagesKHR(device, swapchain, &swapCount, swapchainImages.data());

        swapchainImageViews.resize(swapCount);
        for (size_t i = 0; i < swapchainImages.size(); ++i) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapchainFormat;
            viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY };
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            check(vkCreateImageView(device, &viewInfo, nullptr, &swapchainImageViews[i]),
                  "vkCreateImageView failed");
        }
    }

    VkRenderPass renderPass = VK_NULL_HANDLE;
    {
        VkAttachmentDescription color{};
        color.format = swapchainFormat;
        color.samples = VK_SAMPLE_COUNT_1_BIT;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;

        VkSubpassDependency dep{};
        dep.srcSubpass = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass = 0;
        dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.srcAccessMask = 0;
        dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo rpInfo{};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpInfo.attachmentCount = 1;
        rpInfo.pAttachments = &color;
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;
        rpInfo.dependencyCount = 1;
        rpInfo.pDependencies = &dep;

        check(vkCreateRenderPass(device, &rpInfo, nullptr, &renderPass),
              "vkCreateRenderPass failed");
    }

    auto readFile = [](const char* path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open shader file");
        }
        size_t size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(size);
        file.seekg(0);
        file.read(buffer.data(), size);
        return buffer;
    };

    VkShaderModule vertModule = VK_NULL_HANDLE;
    VkShaderModule fragModule = VK_NULL_HANDLE;
    {
        auto vertCode = readFile("triangle.vert.spv");
        auto fragCode = readFile("triangle.frag.spv");

        VkShaderModuleCreateInfo vertInfo{};
        vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertInfo.codeSize = vertCode.size();
        vertInfo.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());
        check(vkCreateShaderModule(device, &vertInfo, nullptr, &vertModule),
              "vkCreateShaderModule vert failed");

        VkShaderModuleCreateInfo fragInfo{};
        fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragInfo.codeSize = fragCode.size();
        fragInfo.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());
        check(vkCreateShaderModule(device, &fragInfo, nullptr, &fragModule),
              "vkCreateShaderModule frag failed");
    }

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    check(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout),
          "vkCreatePipelineLayout failed");

    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    {
        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = vertModule;
        vertStage.pName = "main";

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = fragModule;
        fragStage.pName = "main";

        VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

        VkPipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo raster{};
        raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster.depthClampEnable = VK_FALSE;
        raster.rasterizerDiscardEnable = VK_FALSE;
        raster.polygonMode = VK_POLYGON_MODE_FILL;
        raster.lineWidth = 1.0f;
        raster.cullMode = VK_CULL_MODE_BACK_BIT;
        raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
        raster.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisample{};
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlend{};
        colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.logicOpEnable = VK_FALSE;
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &colorBlendAttachment;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &raster;
        pipelineInfo.pMultisampleState = &multisample;
        pipelineInfo.pColorBlendState = &colorBlend;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        check(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                        nullptr, &graphicsPipeline),
              "vkCreateGraphicsPipelines failed");
    }

    std::vector<VkFramebuffer> framebuffers;
    {
        framebuffers.resize(swapchainImageViews.size());
        for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
            VkImageView attachments[] = { swapchainImageViews[i] };

            VkFramebufferCreateInfo fbInfo{};
            fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass = renderPass;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments = attachments;
            fbInfo.width = swapchainExtent.width;
            fbInfo.height = swapchainExtent.height;
            fbInfo.layers = 1;

            check(vkCreateFramebuffer(device, &fbInfo, nullptr, &framebuffers[i]),
                  "vkCreateFramebuffer failed");
        }
    }

    VkCommandPool commandPool = VK_NULL_HANDLE;
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = qIndices.graphicsFamily.value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        check(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
              "vkCreateCommandPool failed");
    }

    std::vector<VkCommandBuffer> commandBuffers;
    {
        commandBuffers.resize(framebuffers.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        check(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()),
              "vkAllocateCommandBuffers failed");

        for (size_t i = 0; i < commandBuffers.size(); ++i) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            check(vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
                  "vkBeginCommandBuffer failed");

            VkClearValue clear{};
            clear.color = { { 0.05f, 0.06f, 0.08f, 1.0f } };

            VkRenderPassBeginInfo rpBegin{};
            rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpBegin.renderPass = renderPass;
            rpBegin.framebuffer = framebuffers[i];
            rpBegin.renderArea.offset = { 0, 0 };
            rpBegin.renderArea.extent = swapchainExtent;
            rpBegin.clearValueCount = 1;
            rpBegin.pClearValues = &clear;

            vkCmdBeginRenderPass(commandBuffers[i], &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(commandBuffers[i]);

            check(vkEndCommandBuffer(commandBuffers[i]), "vkEndCommandBuffer failed");
        }
    }

    std::array<VkSemaphore, kMaxFramesInFlight> imageAvailable{};
    std::array<VkSemaphore, kMaxFramesInFlight> renderFinished{};
    std::array<VkFence, kMaxFramesInFlight> inFlightFences{};
    std::vector<VkFence> imagesInFlight(swapchainImages.size(), VK_NULL_HANDLE);

    {
        VkSemaphoreCreateInfo semInfo{};
        semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < kMaxFramesInFlight; ++i) {
            check(vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailable[i]),
                  "vkCreateSemaphore failed");
            check(vkCreateSemaphore(device, &semInfo, nullptr, &renderFinished[i]),
                  "vkCreateSemaphore failed");
            check(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]),
                  "vkCreateFence failed");
        }
    }

    size_t currentFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult acquire = vkAcquireNextImageKHR(
            device, swapchain, UINT64_MAX,
            imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("vkAcquireNextImageKHR failed");
        }

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailable[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = { renderFinished[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        check(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]),
              "vkQueueSubmit failed");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        VkResult present = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (present != VK_SUCCESS && present != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("vkQueuePresentKHR failed");
        }

        currentFrame = (currentFrame + 1) % kMaxFramesInFlight;
    }

    vkDeviceWaitIdle(device);

    for (size_t i = 0; i < kMaxFramesInFlight; ++i) {
        vkDestroySemaphore(device, imageAvailable[i], nullptr);
        vkDestroySemaphore(device, renderFinished[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    for (auto fb : framebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (auto view : swapchainImageViews) {
        vkDestroyImageView(device, view, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    vkDestroyShaderModule(device, vertModule, nullptr);
    vkDestroyShaderModule(device, fragModule, nullptr);

    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
