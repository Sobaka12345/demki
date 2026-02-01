#include "default_renderer.hpp"
#include "renderer_fwd.hpp"

#include <cstring>
#include <surface.hpp>

#include <vector>
#include <vulkan/vulkan_core.h>

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

void DefaultRenderer<Vk>::createSwapchain(Context& ctx) noexcept
{
    const auto& surfaceInfo = ctx.physicalDeviceInUse->surfaceInfo;

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(surfaceInfo.formats);
    VkPresentModeKHR presentMode = choosePresentMode(surfaceInfo.presentModes);
    VkExtent2D extent = chooseExtent(surfaceInfo.capabilities, ctx.iSurface);

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
        .imageExtent = extent,
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

void DefaultRenderer<Vk>::destroySwapchain(Context& ctx) noexcept
{
    for (size_t i = 0; i < ctx.swapchain.size; ++i) {
        vkDestroyImageView(ctx.device, ctx.swapchain.imageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(ctx.device, ctx.swapchain.handle, nullptr);
}

void DefaultRenderer<Vk>::recreateSwapchain(Context& ctx) noexcept
{
    while (!ctx.iSurface->available()) {
        ctx.iSurface->waitForEvents();
    }
    vkDeviceWaitIdle(ctx.device);

    destroySwapchain(ctx);
    createSwapchain(ctx);
}

void DefaultRenderer<Vk>::pickSuitablePhysicalDevices(DefaultRenderer<Vk>::Context& ctx) noexcept
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
}

void DefaultRenderer<Vk>::createDevice(Context& ctx) noexcept
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

void DefaultRenderer<Vk>::destroyDevice(Context& ctx) noexcept
{
    for (size_t i = 0; i < ctx.commandPools.size(); ++i) {
        vkDestroyCommandPool(ctx.device, ctx.commandPools[i], nullptr);
    }
    vkDestroyDevice(ctx.device, nullptr);
}

void DefaultRenderer<Vk>::createSynchronization(Context& ctx) noexcept {
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

void DefaultRenderer<Vk>::destroySynchronization(Context& ctx) noexcept {
    for (size_t i = 0; i < ctx.maxFramesInFlight; ++i)
    {
        vkDestroyFence(ctx.device, ctx.inFlightFences[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.renderWaitSemaphores[i], nullptr);
        vkDestroySemaphore(ctx.device, ctx.renderFinishedSemaphores[i], nullptr);
    }
}

DefaultRenderer<Vk>::Context DefaultRenderer<Vk>::init(gapi::GApiContext<Vk>& gApiContext) 
{
    Context result = { gApiContext };
    pickSuitablePhysicalDevices(result);
    ASSERT(result.suitablePhysicalDevices.size(), "failed to find a suitable GPU");
    
    result.physicalDeviceInUse = &result.suitablePhysicalDevices[0];
    for (size_t i = 1; i < result.suitablePhysicalDevices.size(); ++i) {
        Context::PhysicalDevice* d = &result.suitablePhysicalDevices[i]; 
        if (d->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            d->memoryProperties.memoryHeapCount > result.physicalDeviceInUse->memoryProperties.memoryHeapCount
        ) {
            result.physicalDeviceInUse = d;
        }
    }

    createDevice(result);
    createSynchronization(result);
    createSwapchain(result);
    

    return result;
}

void DefaultRenderer<Vk>::teardown(Context& ctx)
{
    destroySwapchain(ctx);
    destroySynchronization(ctx);
    destroyDevice(ctx);
}

void DefaultRenderer<Vk>::prepareSwapchain(Context& ctx) {
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
    
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::PRESENT][ctx.currentFrameInFlight];
    vkResetCommandBuffer(commandBuffer, 0);
    const auto commandBufferBeginInfo = VkCommandBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pInheritanceInfo = nullptr
    };

    ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS,
        "failed to begin recording command buffer!");
}

void DefaultRenderer<Vk>::presentSwapchain(Context& ctx) {
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::PRESENT][ctx.currentFrameInFlight];
    ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "failed to record command buffer!");

    auto submitInfo =
        VkSubmitInfo {
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

void DefaultRenderer<Vk>::render(Context& context) {
    prepareSwapchain(context);

    presentSwapchain(context);
}

}