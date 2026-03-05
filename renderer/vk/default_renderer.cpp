#include "renderer.hpp"
#include "renderer_fwd.hpp"

#include <array>
#include <cstring>
#include <surface.hpp>
#include <shader.hpp>

#include <vector>
#include <vulkan/vulkan_core.h>

namespace renderer::shaders {
#include "shaders_hpp/shader.frag.spv.hpp"
#include "shaders_hpp/shader.vert.spv.hpp"
}

namespace renderer::__private {

VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const gapi::ISurface* surface)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        const auto [width, height] = surface->framebufferSize();
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

VkSurfaceFormatKHR chooseSurfaceFormat(
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

VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

void DefaultRenderer::createSwapchain(Context& ctx) noexcept
{
    const auto& surfaceInfo = ctx.physicalDeviceInUse->surfaceInfo;

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(surfaceInfo.formats);
    VkPresentModeKHR presentMode = choosePresentMode(surfaceInfo.presentModes);
    ctx.swapchain.extent = chooseExtent(surfaceInfo.capabilities, ctx.iSurface);

    const uint32_t graphicsComputeFamilyIndex = 
        ctx.physicalDeviceInUse->queueFamilies[Context::QueueFamily::Type::GRAPHICS_COMPUTE].index;
    const uint32_t presentFamilyIndex = 
        ctx.physicalDeviceInUse->queueFamilies[Context::QueueFamily::Type::PRESENT].index;

    std::vector<uint32_t> queueFamilyIndices = { graphicsComputeFamilyIndex };
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (graphicsComputeFamilyIndex != presentFamilyIndex)
    {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = {
            graphicsComputeFamilyIndex,
            presentFamilyIndex,
        };
    }

    ctx.swapchain.size = surfaceInfo.capabilities.minImageCount + 1;
    if (surfaceInfo.capabilities.maxImageCount > 0 &&
        ctx.swapchain.size > surfaceInfo.capabilities.maxImageCount)
    {
        ctx.swapchain.size = surfaceInfo.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = ctx.surface,
        .minImageCount = ctx.swapchain.size,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = ctx.swapchain.extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sharingMode,
        .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
        .pQueueFamilyIndices = queueFamilyIndices.data(),
        .preTransform = surfaceInfo.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    ASSERT(VK_SUCCESS == vkCreateSwapchainKHR(ctx.device, &swapchainCreateInfo, nullptr, &ctx.swapchain.handle),
        "failed to create swapchain");

    ctx.swapchain.imageFormat = surfaceFormat.format;
    ASSERT(VK_SUCCESS == vkGetSwapchainImagesKHR(ctx.device, ctx.swapchain.handle, &ctx.swapchain.size, nullptr));
    ctx.swapchain.images.resize(ctx.swapchain.size);
    ASSERT(VK_SUCCESS == vkGetSwapchainImagesKHR(ctx.device, ctx.swapchain.handle, &ctx.swapchain.size, ctx.swapchain.images.data()));
    
    ctx.swapchain.imageViews.resize(ctx.swapchain.size);
    for (size_t i = 0; i < ctx.swapchain.size; ++i)
    {
        auto createInfo = VkImageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = ctx.swapchain.images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceFormat.format,
            .subresourceRange = VkImageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        ASSERT(VK_SUCCESS == vkCreateImageView(ctx.device, &createInfo, nullptr, &ctx.swapchain.imageViews[i]),
            "failed to create swapchain image views");
    }

}

void DefaultRenderer::destroySwapchain(Context& ctx) noexcept
{
    for (size_t i = 0; i < ctx.swapchain.size; ++i) {
        vkDestroyImageView(ctx.device, ctx.swapchain.imageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(ctx.device, ctx.swapchain.handle, nullptr);
}

void DefaultRenderer::recreateSwapchain(Context& ctx) noexcept
{
    while (!ctx.iSurface->available()) {
        ctx.iSurface->waitForEvents();
    }
    vkDeviceWaitIdle(ctx.device);

    destroyFramebuffers(ctx);
    destroySwapchain(ctx);

    createSwapchain(ctx);
    createFramebuffers(ctx);
}
    
void DefaultRenderer::pickSuitablePhysicalDevices(DefaultRenderer::Context& ctx) noexcept
{
    const std::vector<const char *> static s_requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    for (const auto& physicalDevice : ctx.physicalDevices)
    {
        bool found = false;
        for (const auto& requiredExtension : s_requiredDeviceExtensions) 
        {
            found = physicalDevice.availableExtensions.end() !=
                std::find_if(physicalDevice.availableExtensions.begin(), physicalDevice.availableExtensions.end(), 
                    [&requiredExtension] (auto& extension) {
                        return !std::strcmp(extension.extensionName, requiredExtension);
                    });
            if (!found) break;
        }

        if (found && 
            physicalDevice.surfaceInfo.presentModes.size() && 
            physicalDevice.surfaceInfo.formats.size() &&
            physicalDevice.features.samplerAnisotropy == VK_TRUE &&
            physicalDevice.features.sampleRateShading == VK_TRUE &&
            physicalDevice.queueFamilies[Context::QueueFamily::PRESENT].index != Context::QueueFamily::INVALID_QUEUE_FAMILY_INDEX &&
            physicalDevice.queueFamilies[Context::QueueFamily::GRAPHICS_COMPUTE].index != Context::QueueFamily::INVALID_QUEUE_FAMILY_INDEX
        ) {
            ctx.suitablePhysicalDevices.push_back(physicalDevice);
        }
    }

    ASSERT(ctx.suitablePhysicalDevices.size(), "failed to find a suitable GPU");
    
    ctx.physicalDeviceInUse = &ctx.suitablePhysicalDevices[0];
    for (size_t i = 1; i < ctx.suitablePhysicalDevices.size(); ++i) {
        DefaultRenderer::Context::PhysicalDevice* d = &ctx.suitablePhysicalDevices[i]; 
        if (d->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            d->memoryProperties.memoryHeapCount > ctx.physicalDeviceInUse->memoryProperties.memoryHeapCount
        ) {
            ctx.physicalDeviceInUse = d;
        }
    }

    ctx.depthFormat = ctx.physicalDeviceInUse->findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void DefaultRenderer::createDevice(Context& ctx) noexcept
{
    const std::array deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const std::array<float, 1> queuePriorities{ 1.0f };
    const auto& queueFamilies = ctx.physicalDeviceInUse->queueFamilies;
    std::array<VkDeviceQueueCreateInfo, Context::QueueFamily::Type::COUNT> queueCreateInfos;
    std::array<VkCommandPoolCreateInfo, Context::QueueFamily::Type::COUNT> commandPoolCreateInfos;
    
    for (Context::QueueFamily::Type i = Context::QueueFamily::Type::BEGIN; i < Context::QueueFamily::Type::COUNT; ++i)
    {
        queueCreateInfos[i] = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamilies[i].index,
            .queueCount = queuePriorities.size(),
            .pQueuePriorities = queuePriorities.data() 
        };

        commandPoolCreateInfos[i] = VkCommandPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilies[i].index
        };
    }

    const auto deviceFeatures = VkPhysicalDeviceFeatures {
        .sampleRateShading = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

    const auto deviceCreateInfo = VkDeviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .flags = 0,
        .queueCreateInfoCount = queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = deviceExtensions.size(),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures
    };      

    ASSERT(VK_SUCCESS == vkCreateDevice(ctx.physicalDeviceInUse->handle, &deviceCreateInfo, nullptr, &ctx.device),
        "failed to create logical device!");

    for (Context::QueueFamily::Type i = Context::QueueFamily::Type::BEGIN; i < Context::QueueFamily::Type::COUNT; ++i)
    {
        ASSERT(VK_SUCCESS == vkCreateCommandPool(ctx.device, &commandPoolCreateInfos[i], nullptr, &ctx.commandPools[i]));
        // hmmmmm....
        vkGetDeviceQueue(ctx.device, ctx.physicalDeviceInUse->queueFamilies[i].index, 0, &ctx.queues[i]);
        const auto cbAllocateInfo = VkCommandBufferAllocateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = ctx.commandPools[i],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = ctx.maxFramesInFlight,
        };
        ctx.commandBuffers[i].resize(ctx.maxFramesInFlight);
        vkAllocateCommandBuffers(ctx.device, &cbAllocateInfo, ctx.commandBuffers[i].data());
    }

}

void DefaultRenderer::destroyDevice(Context& ctx) noexcept
{
    for (size_t i = 0; i < ctx.commandPools.size(); ++i) {
        vkDestroyCommandPool(ctx.device, ctx.commandPools[i], nullptr);
    }
    vkDestroyDevice(ctx.device, nullptr);
}

void DefaultRenderer::createSynchronization(Context& ctx) noexcept {
    constexpr static auto fenceInfo = VkFenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    constexpr static auto semaphoreInfo = VkSemaphoreCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    ctx.inFlightFences.resize(ctx.maxFramesInFlight);
    ctx.imageAvailableSemaphores.resize(ctx.maxFramesInFlight);
    ctx.renderWaitSemaphores.resize(ctx.maxFramesInFlight);
    ctx.renderFinishedSemaphores.resize(ctx.maxFramesInFlight);
    for (size_t i = 0; i < ctx.maxFramesInFlight; ++i)
    {
        vkCreateFence(ctx.device, &fenceInfo, nullptr, &ctx.inFlightFences[i]);
        vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.imageAvailableSemaphores[i]);
        vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.renderWaitSemaphores[i]);
        vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.renderFinishedSemaphores[i]);
    }
}

void DefaultRenderer::destroySynchronization(Context& ctx) noexcept {
    for (size_t i = 0; i < ctx.maxFramesInFlight; ++i)
    {
        vkDestroyFence(ctx.device, ctx.inFlightFences[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.renderWaitSemaphores[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.renderFinishedSemaphores[i], nullptr);
    }
}

void DefaultRenderer::createAttachments(Context &ctx) noexcept {
    ctx.attachments.references = {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        VkAttachmentReference {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        VkAttachmentReference {
            .attachment = 2,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    ctx.attachments.data.resize(3);
    ctx.attachments.data[0] = VkAttachmentDescription {
        .format = ctx.swapchain.imageFormat,
        .samples = ctx.sampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    // Resolve Attachment
    ctx.attachments.data[1] = VkAttachmentDescription {
        .format = ctx.swapchain.imageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    ctx.attachments.colorAttachmentCount = 1;

    ctx.attachments.data[2] = VkAttachmentDescription {
        .format = ctx.depthFormat,
        .samples = ctx.sampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
}

void DefaultRenderer::destroyAttachments(Context &ctx) noexcept {
    ctx.attachments.data.clear();
    ctx.attachments.colorAttachmentCount = 0;
}

void DefaultRenderer::createRenderPasses(Context& ctx) noexcept {
    const auto subpass = VkSubpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = ctx.attachments.colorAttachmentCount,
        .pColorAttachments = ctx.attachments.colorAttachmentReferences(),
        .pResolveAttachments = ctx.attachments.resolveAttachmentReferences(),
        .pDepthStencilAttachment = ctx.attachments.depthStencilAttachmentReference(),
    };

    const auto dependency = VkSubpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    const auto renderPassCreateInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast<uint32_t>(ctx.attachments.data.size()),
        .pAttachments = ctx.attachments.data.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };      

    ASSERT(VK_SUCCESS == vkCreateRenderPass(ctx.device, &renderPassCreateInfo, nullptr, &ctx.renderPass), 
        "failed to create render pass");
}

void DefaultRenderer::destroyRenderPasses(Context& ctx) noexcept {
    vkDestroyRenderPass(ctx.device, ctx.renderPass, nullptr);
}

void DefaultRenderer::createFramebuffers(Context &ctx) noexcept {
    ctx.framebuffers.resize(ctx.swapchain.size);
    for (size_t i = 0; i < ctx.swapchain.size; ++i) 
    {
        auto& framebufferData = ctx.framebuffers[i];
        
        const auto imageCreateInfo = VkImageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .extent = VkExtent3D{
                .width = ctx.swapchain.extent.width,
                .height = ctx.swapchain.extent.height,
                .depth = 1,
            },
            .mipLevels = 1,
            .arrayLayers = 1,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        const auto attachmentCount = ctx.attachments.data.size();
        framebufferData.imageViewAttachments.resize(attachmentCount);
        framebufferData.images.resize(attachmentCount);
        framebufferData.imageMemory.resize(attachmentCount);
        for (size_t attachmentIdx = 0; attachmentIdx < attachmentCount; ++attachmentIdx) 
        {
            const auto& attachment = ctx.attachments.data[attachmentIdx];
            auto& framebufferImage = framebufferData.images[attachmentIdx];
            auto& framebufferImageMemory = framebufferData.imageMemory[attachmentIdx];
            auto& framebufferImageViewAttachment = framebufferData.imageViewAttachments[attachmentIdx];


            VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            VkFormat format = ctx.swapchain.imageFormat;

            switch (attachment.finalLayout) {
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                framebufferData.images[attachmentIdx] = VK_NULL_HANDLE;
                framebufferData.imageViewAttachments[attachmentIdx] = ctx.swapchain.imageViews[attachmentIdx];
                break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                goto falltrough;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                format = ctx.depthFormat;
                goto falltrough;
            default:
                falltrough:
                auto createInfo = imageCreateInfo;
                createInfo.format = format;
                createInfo.usage = usage;
                createInfo.samples = ctx.sampleCount;
                ASSERT(VK_SUCCESS == vkCreateImage(ctx.device, &createInfo, nullptr, &framebufferImage));

                VkMemoryRequirements memoryRequirements;
                vkGetImageMemoryRequirements(ctx.device, framebufferImage, &memoryRequirements);
                const auto imageAllocateInfo = VkMemoryAllocateInfo {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .allocationSize = memoryRequirements.size,
                    .memoryTypeIndex = ctx.physicalDeviceInUse->findMemoryType(
                        memoryRequirements.memoryTypeBits, 
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                    ),
                };
                ASSERT(VK_SUCCESS == vkAllocateMemory(ctx.device, &imageAllocateInfo, nullptr, &framebufferImageMemory));
                ASSERT(VK_SUCCESS == vkBindImageMemory(ctx.device, framebufferImage, framebufferImageMemory, 0));

                auto imageViewCreateInfo = VkImageViewCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = framebufferImage,
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = format,
                    .subresourceRange = VkImageSubresourceRange {
                        .aspectMask = aspectMask,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    }
                };
                ASSERT(VK_SUCCESS == vkCreateImageView(ctx.device, &imageViewCreateInfo, nullptr, &framebufferImageViewAttachment));

                break;
            }
        }

        const auto framebufferCreateInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = ctx.renderPass,
            .attachmentCount = static_cast<uint32_t>(framebufferData.imageViewAttachments.size()),
            .pAttachments = framebufferData.imageViewAttachments.data(),
            .width = ctx.swapchain.extent.width,
            .height = ctx.swapchain.extent.height,
            .layers = 1,
        };
        vkCreateFramebuffer(ctx.device, &framebufferCreateInfo, nullptr, &ctx.framebuffers[i].handle);
    }
}

void DefaultRenderer::destroyFramebuffers(Context& ctx) noexcept {
    for (const auto& framebufferData : ctx.framebuffers) {
        
        for(size_t i = 0; i < framebufferData.images.size(); ++i) {
            const auto& image = framebufferData.images[i];
            if (image != VK_NULL_HANDLE) {
                vkDestroyImageView(ctx.device, framebufferData.imageViewAttachments[i], nullptr);
                vkFreeMemory(ctx.device, framebufferData.imageMemory[i], nullptr);
                vkDestroyImage(ctx.device, image, nullptr);
            } 
        }

        vkDestroyFramebuffer(ctx.device, framebufferData.handle, nullptr);
    }
    ctx.framebuffers.clear();
}

void DefaultRenderer::createPipeline(Context& ) noexcept {
    constexpr auto shaderVertArray = std::to_array(shaders::shader_vert_spv);
    constexpr auto shaderFragArray = std::to_array(shaders::shader_frag_spv);
    constexpr gapi::Shader<Vk> shader1 = gapi::parseShader<Vk, shaderVertArray>();
    constexpr gapi::Shader<Vk> shader2 = gapi::parseShader<Vk, shaderFragArray>();
    constexpr auto t =  shader1.resourceTypes[0];
    t;
    std::cout << "HELO" << shader1.stage << std::endl;
    std::cout << "HELO" << shader2.stage << std::endl;
}

void DefaultRenderer::destroyPipeline(Context& ) noexcept {
}

void DefaultRenderer::prepareSwapchain(Context& ctx) noexcept
{
    vkWaitForFences(ctx.device, 1, ctx.inFlightFences.data(), VK_TRUE, UINT64_MAX);
    
    VkResult result = vkAcquireNextImageKHR(ctx.device, ctx.swapchain.handle, UINT64_MAX,
        ctx.imageAvailableSemaphores[ctx.currentFrameInFlight], VK_NULL_HANDLE, &ctx.swapchain.currentImage);

    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swap chain image!");

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain(ctx);
        return;
    }

    vkResetFences(ctx.device, 1, &ctx.inFlightFences[ctx.currentFrameInFlight]);
    
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::GRAPHICS_COMPUTE][ctx.currentFrameInFlight];
    vkResetCommandBuffer(commandBuffer, 0);
    const auto commandBufferBeginInfo = VkCommandBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pInheritanceInfo = nullptr
    };

    ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS,
        "failed to begin recording command buffer!");
}

void DefaultRenderer::presentSwapchain(Context& ctx) noexcept
{
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::GRAPHICS_COMPUTE][ctx.currentFrameInFlight];
    ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "failed to record command buffer!");

    auto submitInfo =
        VkSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &ctx.renderFinishedSemaphores[ctx.currentFrameInFlight]
        };

    const std::array<VkPipelineStageFlags, 1> waitStages = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    const std::array<VkSemaphore, 1> waitSemaphores = { 
        ctx.imageAvailableSemaphores[ctx.currentFrameInFlight] 
    };

    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    const auto queue = ctx.queues[Context::QueueFamily::GRAPHICS_COMPUTE];

    ASSERT(vkQueueSubmit(queue, 1, &submitInfo, ctx.inFlightFences[ctx.currentFrameInFlight]) == VK_SUCCESS,
        "failed to submit draw command buffer!");

    const auto queuePresentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &ctx.renderFinishedSemaphores[ctx.currentFrameInFlight],
        .swapchainCount = 1,
        .pSwapchains = &ctx.swapchain.handle,
        .pImageIndices = &ctx.swapchain.currentImage,
    };

    VkResult result = vkQueuePresentKHR(queue, &queuePresentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || /*m_needRecreate ||*/
        !ctx.iSurface->available())
    {
        recreateSwapchain(ctx);
    }
    else if (result != VK_SUCCESS)
    {
        ASSERT(false, "failed to present swap chain image!");
    }

    ctx.currentFrameInFlight = (ctx.currentFrameInFlight + 1) % ctx.maxFramesInFlight;
}

void DefaultRenderer::prepareFrame(Context& ctx) noexcept {
    prepareSwapchain(ctx);
}

void DefaultRenderer::presentFrame(Context& ctx) noexcept
{
    presentSwapchain(ctx);
}

void DefaultRenderer::prepareRenderPass(Context& ) noexcept
{
    
}

void DefaultRenderer::presentRenderPass(Context& ) noexcept
{
 
}


template <>
DefaultRenderer::Context setupRenderer<Vk, DefaultRenderer>(gapi::GApiContext<Vk>& ctx) noexcept
{
    DefaultRenderer::Context result = { ctx };
    DefaultRenderer::pickSuitablePhysicalDevices(result);

    DefaultRenderer::createDevice(result);
    DefaultRenderer::createSynchronization(result);
    DefaultRenderer::createSwapchain(result);
    DefaultRenderer::createAttachments(result);
    DefaultRenderer::createRenderPasses(result);
    DefaultRenderer::createFramebuffers(result);

    DefaultRenderer::createPipeline(result);


    return result;
}

template <>
void teardownRenderer<Vk, DefaultRenderer>(DefaultRenderer::Context& ctx) noexcept
{

    DefaultRenderer::destroyPipeline(ctx);

    DefaultRenderer::destroyFramebuffers(ctx);
    DefaultRenderer::destroyRenderPasses(ctx);
    DefaultRenderer::destroyAttachments(ctx);
    DefaultRenderer::destroySwapchain(ctx);
    DefaultRenderer::destroySynchronization(ctx);
    DefaultRenderer::destroyDevice(ctx);
}

}