#include "renderer.hpp"
#include "renderer_fwd.hpp"

#include <buffer.hpp>

#include <array>
#include <cstring>
#include <surface.hpp>
#include <shader.hpp>
#include "shaders/shared_types.hpp"

#include <numeric>
#include <vector>
#include <map>
#include <vulkan/vulkan_core.h>

namespace renderer::__private {

namespace shaders {
#include "shaders_hpp/shader.frag.spv.hpp"
#include "shaders_hpp/shader.vert.spv.hpp"



struct DescriptorSetBindings {
    uint32_t setId{};
    uint32_t bindingCount{};
    std::array<VkDescriptorSetLayoutBinding, 32> bindings{};
};

struct ShaderDescriptorSet {
    std::array<VkDescriptorType, 32> types{};
    std::array<VkShaderStageFlags, 32> flags{};
    uint32_t mask{};
};

template <auto ...ShaderValue>
consteval auto mergeShaderResources()
{
    std::array<ShaderDescriptorSet, GLSL_SET_COUNT> descriptorSets{};

    const auto res = [&](const auto& shaderMeta) {
        for (size_t setId = 0; setId < descriptorSets.size(); ++setId) 
        {
            for (uint32_t binding = 0; binding < descriptorSets[setId].types.size(); ++binding)
            {
                const uint32_t bindingMask = 1u << binding;
                if ((shaderMeta.resourceMasks[setId] & bindingMask) == 0)
                {
                    continue;
                }
    
                if ((descriptorSets[setId].mask & bindingMask) != 0)
                {
                    CONSTEVAL_ASSERT(descriptorSets[setId].types[binding] == shaderMeta.resources[setId][binding]);
                }
                else
                {
                    descriptorSets[setId].types[binding] = shaderMeta.resources[setId][binding];
                    descriptorSets[setId].mask |= bindingMask;
                }
    
                descriptorSets[setId].flags[binding] |= shaderMeta.stage;
            }
        }
    };

    (res(ShaderValue), ...);

    return descriptorSets;
}

template <auto ...ShaderValue>
consteval auto descriptorSetBindings()
{
    constexpr std::array<ShaderDescriptorSet, GLSL_SET_COUNT> descriptorSets = mergeShaderResources<ShaderValue...>();
    std::array<DescriptorSetBindings, GLSL_SET_COUNT> setBindings;
    size_t resSetBindingId = 0;
    for (size_t setId = 0; setId < descriptorSets.size(); ++setId)
    {
        for (uint32_t binding = 0; binding < descriptorSets[setId].types.size(); ++binding)
        {
            const uint32_t bindingMask = 1u << binding;
            if ((descriptorSets[setId].mask & bindingMask) == 0)
            {
                continue;
            }

            setBindings[resSetBindingId].bindings[setBindings[resSetBindingId].bindingCount] = VkDescriptorSetLayoutBinding {
                .binding = binding,
                .descriptorType = descriptorSets[setId].types[binding],
                .descriptorCount = 1,
                .stageFlags = descriptorSets[setId].flags[binding],
                .pImmutableSamplers = nullptr,
            };
            setBindings[resSetBindingId].bindingCount += 1;
        }
        if (setBindings[resSetBindingId].bindingCount) {
            setBindings[resSetBindingId].setId = setId;
            ++resSetBindingId;
        }
    }

    return setBindings;
}

template <auto ... ShaderValue>
consteval auto shaderModuleCreateInfos()
{
    std::array<VkShaderModuleCreateInfo, sizeof...(ShaderValue)> result = {
        VkShaderModuleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = ShaderValue.spirv.size() * sizeof(typename decltype(ShaderValue.spirv)::value_type),
            .pCode = ShaderValue.spirv.data(),
        }...
    };

    return result;
}

template <auto ...ShaderValue>
consteval auto parseShaders()
{
    struct ResultType {
        size_t shaderCount = sizeof...(ShaderValue);
        std::array<VkShaderStageFlagBits, sizeof...(ShaderValue)> stages = { ShaderValue.stage... };
        std::array<DescriptorSetBindings, GLSL_SET_COUNT> descriptorSetBindings{};
        std::array<VkShaderModuleCreateInfo, sizeof...(ShaderValue)> moduleCreateInfos{};
    } result;

    result.descriptorSetBindings = descriptorSetBindings<ShaderValue...>();
    result.moduleCreateInfos = shaderModuleCreateInfos<ShaderValue...>();
    
    return result;
}

constexpr static auto MetaData = parseShaders<
    gapi::Shader<Vk, std::to_array(shaders::shader_vert_spv)>{},
    gapi::Shader<Vk, std::to_array(shaders::shader_frag_spv)>{}
>();

}

namespace {

constexpr VkDeviceSize staticMeshBufferSize = 1024ull * 1024ull * 20ull;

constexpr VkMemoryPropertyFlags staticMeshBufferMemoryProperties =
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

void allocateAndBindBufferMemory(
    gapi::GApiContext<Vk>& ctx,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    gapi::Buffer<Vk>& buffer) noexcept
{
    buffer.device = ctx.device;

    VkBufferCreateInfo bci = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .flags = 0,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    ASSERT(VK_SUCCESS == vkCreateBuffer(ctx.device, &bci, nullptr, &buffer.handle));

    VkMemoryRequirements requirements {};
    vkGetBufferMemoryRequirements(ctx.device, buffer.handle, &requirements);

    VkMemoryAllocateInfo mai {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = ctx.physicalDeviceInUse->findMemoryType(
            requirements.memoryTypeBits, 
            staticMeshBufferMemoryProperties)
    };

    ASSERT(VK_SUCCESS == vkAllocateMemory(ctx.device, &mai, nullptr, &buffer.memory));
    ASSERT(VK_SUCCESS == vkBindBufferMemory(ctx.device, buffer.handle, buffer.memory, 0));
    ASSERT(VK_SUCCESS == vkMapMemory(ctx.device, buffer.memory, 0, VK_WHOLE_SIZE, 0, &buffer.mappedMemory));

    buffer.byteSize = requirements.size;
    buffer.memoryProperties = staticMeshBufferMemoryProperties;
}

void destroyBuffer(gapi::Buffer<Vk>& buffer) noexcept
{
    if (buffer.mappedMemory != nullptr)
    {
        vkUnmapMemory(buffer.device, buffer.memory);
        buffer.mappedMemory = nullptr;
    }

    if (buffer.handle != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(buffer.device, buffer.handle, nullptr);
        buffer.handle = VK_NULL_HANDLE;
    }

    if (buffer.memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(buffer.device, buffer.memory, nullptr);
        buffer.memory = VK_NULL_HANDLE;
    }

    buffer.device = VK_NULL_HANDLE;
    buffer.byteSize = 0;
    buffer.memoryProperties = 0;
}

void writeDefaultStaticMeshData(DefaultRenderer::Context& ctx) noexcept
{
    constexpr auto vertices = std::to_array<Vertex3DColoredTextured>({
        Vertex3DColoredTextured {
            .pos = vec3 { 0.0f, -0.5f, 0.0f },
            .color = vec3 { 1.0f, 0.2f, 0.2f },
            .texture = vec2 { 0.0f, 0.0f },
        },
        Vertex3DColoredTextured {
            .pos = vec3 { 0.5f, 0.5f, 0.0f },
            .color = vec3 { 0.2f, 1.0f, 0.2f },
            .texture = vec2 { 1.0f, 0.0f },
        },
        Vertex3DColoredTextured {
            .pos = vec3 { -0.5f, 0.5f, 0.0f },
            .color = vec3 { 0.2f, 0.4f, 1.0f },
            .texture = vec2 { 0.0f, 1.0f },
        },
    });
    constexpr auto indices = std::to_array<IndexType>({ 0u, 1u, 2u });

    ctx.staticVertexBuffer.buffer.write(vertices.data(), sizeof(vertices), 0);
    ctx.staticIndexBuffer.buffer.write(indices.data(), sizeof(indices), 0);
}

} // namespace

VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const gapi::ISurface* surface)
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

    destroySynchronization(ctx);
    destroyStaticMeshDescriptors(ctx);
    destroyPipeline(ctx);
    destroyFramebuffers(ctx);
    destroyRenderPasses(ctx);
    destroyAttachments(ctx);
    destroySwapchain(ctx);

    ctx.physicalDeviceInUse->fetchSurfaceInfo(ctx.surface);

    createSwapchain(ctx);
    createSynchronization(ctx);
    createAttachments(ctx);
    createRenderPasses(ctx);
    createFramebuffers(ctx);
    createPipeline(ctx);
    createStaticMeshDescriptors(ctx);
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
    ctx.renderFinishedSemaphores.resize(ctx.swapchain.size);
    for (size_t i = 0; i < ctx.maxFramesInFlight; ++i)
    {
        ASSERT(VK_SUCCESS == vkCreateFence(ctx.device, &fenceInfo, nullptr, &ctx.inFlightFences[i]));
        ASSERT(VK_SUCCESS == vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.imageAvailableSemaphores[i]));
        ASSERT(VK_SUCCESS == vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.renderWaitSemaphores[i]));
    }

    for (size_t i = 0; i < ctx.swapchain.size; ++i)
    {
        ASSERT(VK_SUCCESS == vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.renderFinishedSemaphores[i]));
    }
}

void DefaultRenderer::destroySynchronization(Context& ctx) noexcept {
    for (const auto& fence : ctx.inFlightFences)
    {
        vkDestroyFence(ctx.device, fence, nullptr);
    }
    for (const auto& semaphore : ctx.imageAvailableSemaphores)
    {
        vkDestroySemaphore(ctx.device, semaphore, nullptr);
    }
    for (const auto& semaphore : ctx.renderWaitSemaphores)
    {
        vkDestroySemaphore(ctx.device, semaphore, nullptr);
    }
    for (const auto& semaphore : ctx.renderFinishedSemaphores)
    {
        vkDestroySemaphore(ctx.device, semaphore, nullptr);
    }

    ctx.inFlightFences.clear();
    ctx.imageAvailableSemaphores.clear();
    ctx.renderWaitSemaphores.clear();
    ctx.renderFinishedSemaphores.clear();
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
                framebufferData.imageViewAttachments[attachmentIdx] = ctx.swapchain.imageViews[i];
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

void DefaultRenderer::createPipeline(Context& ctx) noexcept {
    for (size_t i = 0; i < shaders::MetaData.descriptorSetBindings.size(); ++i)
    {
        if (!shaders::MetaData.descriptorSetBindings[i].bindingCount) continue;
        
        const auto descriptorSetLayoutCreateInfo = VkDescriptorSetLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = shaders::MetaData.descriptorSetBindings[i].bindingCount,
            .pBindings = shaders::MetaData.descriptorSetBindings[i].bindings.data(),
        };

        auto& ref = ctx.descriptorSetLayouts.emplace_back(VK_NULL_HANDLE);
        ASSERT(VK_SUCCESS == vkCreateDescriptorSetLayout(ctx.device, &descriptorSetLayoutCreateInfo, nullptr, &ref),
            "failed to create descriptor set layout");
    }

    const auto pipelineLayoutCreateInfo = VkPipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(ctx.descriptorSetLayouts.size()),
        .pSetLayouts = ctx.descriptorSetLayouts.data(),
    };
    ASSERT(VK_SUCCESS == vkCreatePipelineLayout(ctx.device, &pipelineLayoutCreateInfo, nullptr, &ctx.pipelineLayout),
        "failed to create pipeline layout");

    std::array<VkShaderModule, shaders::MetaData.shaderCount> shaderModules{};
    std::array<VkPipelineShaderStageCreateInfo, shaders::MetaData.shaderCount> shaderStages{};
    for (size_t i = 0; i < shaders::MetaData.moduleCreateInfos.size(); ++i) {
        ASSERT(VK_SUCCESS == vkCreateShaderModule(ctx.device, 
                &shaders::MetaData.moduleCreateInfos[i], 
                nullptr, 
                &shaderModules[i]
            ),
            "failed to create shader module");

        shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[i].stage = shaders::MetaData.stages[i];
        shaderStages[i].module = shaderModules[i];
        shaderStages[i].pName = "main";
    }


    const auto vertexInputState = VkPipelineVertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    const auto inputAssemblyState = VkPipelineInputAssemblyStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const auto viewport = VkViewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(ctx.swapchain.extent.width),
        .height = static_cast<float>(ctx.swapchain.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const auto scissor = VkRect2D{
        .offset = { 0, 0 },
        .extent = ctx.swapchain.extent,
    };

    const auto viewportState = VkPipelineViewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    const auto rasterizationState = VkPipelineRasterizationStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f,
    };

    const auto multisampleState = VkPipelineMultisampleStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = ctx.sampleCount,
        .sampleShadingEnable = VK_TRUE,
        .minSampleShading = 0.2f,
    };

    const auto depthStencilState = VkPipelineDepthStencilStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    const auto colorBlendAttachment = VkPipelineColorBlendAttachmentState{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const auto colorBlendState = VkPipelineColorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };

    const auto graphicsPipelineCreateInfo = VkGraphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &colorBlendState,
        .layout = ctx.pipelineLayout,
        .renderPass = ctx.renderPass,
        .subpass = 0,
    };

    ASSERT(VK_SUCCESS == vkCreateGraphicsPipelines(ctx.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &ctx.pipeline),
        "failed to create graphics pipeline");

        
    for (size_t i = 0; i < shaderModules.size(); ++i) {
        vkDestroyShaderModule(ctx.device, shaderModules[i], nullptr);
    }
}

void DefaultRenderer::destroyPipeline(Context& ctx) noexcept {
    vkDestroyPipeline(ctx.device, ctx.pipeline, nullptr);
    vkDestroyPipelineLayout(ctx.device, ctx.pipelineLayout, nullptr);

    for (auto layout: ctx.descriptorSetLayouts)
        vkDestroyDescriptorSetLayout(ctx.device, layout, nullptr);

    ctx.pipeline = VK_NULL_HANDLE;
    ctx.pipelineLayout = VK_NULL_HANDLE;
    ctx.descriptorSetLayouts.clear();
}

void DefaultRenderer::createStaticMeshDescriptors(Context& ctx) noexcept
{
    DASSERT(!ctx.descriptorSetLayouts.empty(), "missing descriptor set layouts for static mesh buffers");

    std::map<VkDescriptorType, uint32_t> descriptorTypeCounts;
    for (const auto& setBindings : shaders::MetaData.descriptorSetBindings)
    {
        for (uint32_t bindingId = 0; bindingId < setBindings.bindingCount; ++bindingId)
        {
            const auto& binding = setBindings.bindings[bindingId];
            descriptorTypeCounts[binding.descriptorType] += binding.descriptorCount;
        }
    }

    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.reserve(descriptorTypeCounts.size());
    for (const auto& [descriptorType, descriptorCount] : descriptorTypeCounts)
    {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = descriptorType,
            .descriptorCount = descriptorCount,
        });
    }

    const auto descriptorPoolCreateInfo = VkDescriptorPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(ctx.descriptorSetLayouts.size()),
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };
    ASSERT(VK_SUCCESS == vkCreateDescriptorPool(ctx.device, &descriptorPoolCreateInfo, nullptr, &ctx.descriptorPool),
        "failed to create descriptor pool");

    ctx.descriptorSets.resize(ctx.descriptorSetLayouts.size());
    const auto descriptorSetAllocateInfo = VkDescriptorSetAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = ctx.descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(ctx.descriptorSetLayouts.size()),
        .pSetLayouts = ctx.descriptorSetLayouts.data(),
    };
    ASSERT(VK_SUCCESS == vkAllocateDescriptorSets(ctx.device, &descriptorSetAllocateInfo, ctx.descriptorSets.data()),
        "failed to allocate descriptor sets");

    ASSERT(!ctx.descriptorSets.empty());

    const VkDescriptorBufferInfo vertexBufferInfo{
        .buffer = ctx.staticVertexBuffer.buffer.handle,
        .offset = 0,
        .range = ctx.staticVertexBuffer.buffer.byteSize,
    };
    const VkDescriptorBufferInfo indexBufferInfo{
        .buffer = ctx.staticIndexBuffer.buffer.handle,
        .offset = 0,
        .range = ctx.staticIndexBuffer.buffer.byteSize,
    };

    const std::array descriptorWrites = {
        VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = ctx.descriptorSets[0],
            .dstBinding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &vertexBufferInfo,
        },
        VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = ctx.descriptorSets[0],
            .dstBinding = 1,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &indexBufferInfo,
        },
    };

    vkUpdateDescriptorSets(ctx.device,
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        0,
        nullptr);
}

void DefaultRenderer::destroyStaticMeshDescriptors(Context& ctx) noexcept
{
    ctx.descriptorSets.clear();

    if (ctx.descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(ctx.device, ctx.descriptorPool, nullptr);
        ctx.descriptorPool = VK_NULL_HANDLE;
    }
}

void DefaultRenderer::createMeshBuffers(Context &ctx) noexcept {
    
    allocateAndBindBufferMemory(
        ctx,
        staticMeshBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        ctx.staticVertexBuffer.buffer);
    allocateAndBindBufferMemory(
        ctx,
        staticMeshBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        ctx.staticIndexBuffer.buffer);

    writeDefaultStaticMeshData(ctx);
}

void DefaultRenderer::destroyMeshBuffers(Context& ctx) noexcept {
    destroyBuffer(ctx.staticVertexBuffer.buffer);
    destroyBuffer(ctx.staticIndexBuffer.buffer);
    ctx.staticVertexBuffer.offset = 0;
    ctx.staticIndexBuffer.offset = 0;
}

void DefaultRenderer::prepareSwapchain(Context& ctx) noexcept
{
    ctx.swapchain.currentImage = UINT32_MAX;
    vkWaitForFences(ctx.device, 1, &ctx.inFlightFences[ctx.currentFrameInFlight], VK_TRUE, UINT64_MAX);
    
    VkResult result = vkAcquireNextImageKHR(ctx.device, ctx.swapchain.handle, UINT64_MAX,
        ctx.imageAvailableSemaphores[ctx.currentFrameInFlight], VK_NULL_HANDLE, &ctx.swapchain.currentImage);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain(ctx);
        return;
    }
    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swap chain image!");

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
            .pSignalSemaphores = &ctx.renderFinishedSemaphores[ctx.swapchain.currentImage]
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

    const auto graphicsQueue = ctx.queues[Context::QueueFamily::GRAPHICS_COMPUTE];

    ASSERT(vkQueueSubmit(graphicsQueue, 1, &submitInfo, ctx.inFlightFences[ctx.currentFrameInFlight]) == VK_SUCCESS,
        "failed to submit draw command buffer!");

    const auto queuePresentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &ctx.renderFinishedSemaphores[ctx.swapchain.currentImage],
        .swapchainCount = 1,
        .pSwapchains = &ctx.swapchain.handle,
        .pImageIndices = &ctx.swapchain.currentImage,
    };

    const auto presentQueue = ctx.queues[Context::QueueFamily::PRESENT];
    VkResult result = vkQueuePresentKHR(presentQueue, &queuePresentInfo);

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
    prepareRenderPass(ctx);

    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::GRAPHICS_COMPUTE][ctx.currentFrameInFlight];
    DASSERT(!ctx.descriptorSets.empty(), "static mesh descriptor set was not allocated");
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx.pipeline);
    vkCmdBindDescriptorSets(commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        ctx.pipelineLayout,
        0,
        static_cast<uint32_t>(ctx.descriptorSets.size()),
        ctx.descriptorSets.data(),
        0,
        nullptr);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void DefaultRenderer::presentFrame(Context& ctx) noexcept
{
    presentRenderPass(ctx);
    presentSwapchain(ctx);
}

void DefaultRenderer::prepareRenderPass(Context& ctx) noexcept
{
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::GRAPHICS_COMPUTE][ctx.currentFrameInFlight];
    const std::array clearValues = {
        VkClearValue{ .color = { .float32 = { 0.05f, 0.07f, 0.10f, 1.0f } } },
        VkClearValue{ .color = { .float32 = { 0.05f, 0.07f, 0.10f, 1.0f } } },
        VkClearValue{ .depthStencil = { 1.0f, 0 } },
    };

    const auto renderPassBeginInfo = VkRenderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = ctx.renderPass,
        .framebuffer = ctx.framebuffers[ctx.swapchain.currentImage].handle,
        .renderArea = {
            .offset = { 0, 0 },
            .extent = ctx.swapchain.extent,
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void DefaultRenderer::presentRenderPass(Context& ctx) noexcept
{
    auto commandBuffer = ctx.commandBuffers[Context::QueueFamily::GRAPHICS_COMPUTE][ctx.currentFrameInFlight];
    vkCmdEndRenderPass(commandBuffer);
}


template <>
DefaultRenderer::Context setupRenderer<Vk, DefaultRenderer>(gapi::GApiContext<Vk>& ctx) noexcept
{
    DefaultRenderer::Context result = { ctx };
    DefaultRenderer::pickSuitablePhysicalDevices(result);

    DefaultRenderer::createDevice(result);
    DefaultRenderer::createSwapchain(result);
    DefaultRenderer::createSynchronization(result);
    DefaultRenderer::createAttachments(result);
    DefaultRenderer::createRenderPasses(result);
    DefaultRenderer::createFramebuffers(result);

    DefaultRenderer::createPipeline(result);
    DefaultRenderer::createMeshBuffers(result);
    DefaultRenderer::createStaticMeshDescriptors(result);

    return result;
}

template <>
void teardownRenderer<Vk, DefaultRenderer>(DefaultRenderer::Context& ctx) noexcept
{
    vkDeviceWaitIdle(ctx.device);

    DefaultRenderer::destroyStaticMeshDescriptors(ctx);
    DefaultRenderer::destroyMeshBuffers(ctx);
    DefaultRenderer::destroyPipeline(ctx);

    DefaultRenderer::destroyFramebuffers(ctx);
    DefaultRenderer::destroyRenderPasses(ctx);
    DefaultRenderer::destroyAttachments(ctx);
    DefaultRenderer::destroySwapchain(ctx);
    DefaultRenderer::destroySynchronization(ctx);
    DefaultRenderer::destroyDevice(ctx);
}

}
