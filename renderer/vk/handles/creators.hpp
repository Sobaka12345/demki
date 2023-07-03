#pragma once

#include <array>
#include <concepts>

#include "utils.hpp"

namespace vk { namespace handles {

template <typename T>
concept IsArrayContainer =
    requires(const T& o) {
        o.data();
        o.size();
    };

BEGIN_DECLARE_VKSTRUCT(SubmitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(uint32_t, waitSemaphoreCount)
    VKSTRUCT_PROPERTY(const VkSemaphore*, pWaitSemaphores)
    VKSTRUCT_PROPERTY(const VkPipelineStageFlags*, pWaitDstStageMask)
    VKSTRUCT_PROPERTY(uint32_t, commandBufferCount)
    VKSTRUCT_PROPERTY(const VkCommandBuffer*, pCommandBuffers)
    VKSTRUCT_PROPERTY(uint32_t, signalSemaphoreCount)
    VKSTRUCT_PROPERTY(const VkSemaphore*, pSignalSemaphores)
END_DECLARE_VKSTRUCT()

inline VkDeviceQueueCreateInfo deviceQueueCreateInfo(uint32_t queueFamilyIndex,
    const std::span<const float> queuePriorities,
    VkDeviceQueueCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkDeviceQueueCreateInfo result;
    result.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    result.queueFamilyIndex = queueFamilyIndex;
    result.queueCount = static_cast<uint32_t>(queuePriorities.size());
    result.pQueuePriorities = queuePriorities.data();
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkDeviceCreateInfo deviceCreateInfo(
    const std::span<VkDeviceQueueCreateInfo> queueCreateInfos,
    const std::span<const char* const> enabledExtensionNames,
    const std::span<const char* const> enabledLayerNames = {},
    const VkPhysicalDeviceFeatures* pEnabledFeatures = nullptr,
    VkDeviceCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkDeviceCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    result.queueCreateInfoCount = static_cast<size_t>(queueCreateInfos.size());
    result.pQueueCreateInfos = queueCreateInfos.data();
    result.enabledLayerCount = static_cast<size_t>(enabledLayerNames.size());
    result.ppEnabledLayerNames = enabledLayerNames.data();
    result.enabledExtensionCount = static_cast<size_t>(enabledExtensionNames.size());
    result.ppEnabledExtensionNames = enabledExtensionNames.data();
    result.pEnabledFeatures = pEnabledFeatures;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkBufferCopy bufferCopy(
    VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0)
{
    VkBufferCopy result{};
    result.size = size;
    result.srcOffset = srcOffset;
    result.dstOffset = dstOffset;

    return result;
}

inline constexpr VkImageSubresourceLayers imageSubresourceLayers(
    VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t baseArrayLayer, uint32_t layerCount)
{
    VkImageSubresourceLayers result;
    result.aspectMask = aspectMask;
    result.mipLevel = mipLevel;
    result.baseArrayLayer = baseArrayLayer;
    result.layerCount = layerCount;

    return result;
}

inline constexpr VkBufferImageCopy bufferImageCopy(VkDeviceSize bufferOffset,
    uint32_t bufferRowLength,
    uint32_t bufferImageHeight,
    VkImageSubresourceLayers imageSubresource,
    VkOffset3D imageOffset,
    VkExtent3D imageExtent)
{
    VkBufferImageCopy result{};
    result.bufferOffset = bufferOffset;
    result.bufferRowLength = bufferRowLength;
    result.bufferImageHeight = bufferImageHeight;
    result.imageSubresource = imageSubresource;
    result.imageOffset = imageOffset;
    result.imageExtent = imageExtent;

    return result;
}

inline constexpr VkMemoryAllocateInfo memoryAllocateInfo(
    VkDeviceSize size, uint32_t memoryTypeIndex, const void* pNext = nullptr)
{
    VkMemoryAllocateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    result.allocationSize = size;
    result.memoryTypeIndex = memoryTypeIndex;
    result.pNext = pNext;

    return result;
}

inline constexpr VkMappedMemoryRange mappedMemoryRange(
    VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, const void* pNext = nullptr)
{
    VkMappedMemoryRange result{};
    result.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    result.memory = memory;
    result.size = size;
    result.offset = offset;
    result.pNext = pNext;

    return result;
}

inline constexpr VkCommandPoolCreateInfo commandPoolCreateInfo(
    uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, const void* pNext = nullptr)
{
    VkCommandPoolCreateInfo result;
    result.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    result.queueFamilyIndex = queueFamilyIndex;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool,
    VkCommandBufferLevel level,
    uint32_t commandBufferCount,
    const void* pNext = nullptr)
{
    VkCommandBufferAllocateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    result.level = level;
    result.commandPool = commandPool;
    result.commandBufferCount = commandBufferCount;
    result.pNext = pNext;

    return result;
}

inline constexpr VkCommandBufferBeginInfo commandBufferBeginInfo(
    const VkCommandBufferInheritanceInfo* pInheritanceInfo,
    VkCommandBufferUsageFlags flags = 0,
    const void* pNext = nullptr)
{
    VkCommandBufferBeginInfo result;
    result.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    result.pInheritanceInfo = pInheritanceInfo;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

BEGIN_DECLARE_VKSTRUCT(RenderPassBeginInfo, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkRenderPass, renderPass)
    VKSTRUCT_PROPERTY(VkFramebuffer, framebuffer)
    VKSTRUCT_PROPERTY(VkRect2D, renderArea)
    VKSTRUCT_PROPERTY(uint32_t, clearValueCount)
    VKSTRUCT_PROPERTY(const VkClearValue*, pClearValues)
END_DECLARE_VKSTRUCT()

inline constexpr VkRenderPassBeginInfo renderPassBeginInfo(VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkRect2D renderArea,
    std::span<const VkClearValue> clearValues = {},
    const void* pNext = nullptr)
{
    VkRenderPassBeginInfo result{};
    result.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    result.renderPass = renderPass;
    result.framebuffer = framebuffer;
    result.clearValueCount = static_cast<uint32_t>(clearValues.size());
    result.pClearValues = clearValues.data();
    result.renderArea = renderArea;
    result.pNext = pNext;

    return result;
}

inline constexpr VkShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t codeSize,
    const uint32_t* codeData)
{
    VkShaderModuleCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    result.codeSize = codeSize;
    result.pCode = codeData;

    return result;
}

inline VkShaderModule shaderModule(VkDevice device, const std::vector<char>& code)
{
    const VkShaderModuleCreateInfo createInfo = shaderModuleCreateInfo(
        static_cast<uint32_t>(code.size()), reinterpret_cast<const uint32_t*>(code.data()));

    VkShaderModule result = VK_NULL_HANDLE;
    ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &result) == VK_SUCCESS);

    return result;
}

BEGIN_DECLARE_VKSTRUCT(PipelineViewportStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineViewportStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, viewportCount)
    VKSTRUCT_PROPERTY(const VkViewport*, pViewports)
    VKSTRUCT_PROPERTY(uint32_t, scissorCount)
    VKSTRUCT_PROPERTY(const VkRect2D*, pScissors)
END_DECLARE_VKSTRUCT()


BEGIN_DECLARE_VKSTRUCT(PipelineDynamicStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineDynamicStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, dynamicStateCount)
    VKSTRUCT_PROPERTY(const VkDynamicState*, pDynamicStates)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineInputAssemblyStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineInputAssemblyStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkPrimitiveTopology, topology)
    VKSTRUCT_PROPERTY(VkBool32, primitiveRestartEnable)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineRasterizationStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineRasterizationStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, depthClampEnable)
    VKSTRUCT_PROPERTY(VkBool32, rasterizerDiscardEnable)
    VKSTRUCT_PROPERTY(VkPolygonMode, polygonMode)
    VKSTRUCT_PROPERTY(VkCullModeFlags, cullMode)
    VKSTRUCT_PROPERTY(VkFrontFace, frontFace)
    VKSTRUCT_PROPERTY(VkBool32, depthBiasEnable)
    VKSTRUCT_PROPERTY(float, depthBiasConstantFactor)
    VKSTRUCT_PROPERTY(float, depthBiasClamp)
    VKSTRUCT_PROPERTY(float, depthBiasSlopeFactor)
    VKSTRUCT_PROPERTY(float, lineWidth)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineMultisampleStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineMultisampleStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, rasterizationSamples)
    VKSTRUCT_PROPERTY(VkBool32, sampleShadingEnable)
    VKSTRUCT_PROPERTY(float, minSampleShading)
    VKSTRUCT_PROPERTY(const VkSampleMask*, pSampleMask)
    VKSTRUCT_PROPERTY(VkBool32, alphaToCoverageEnable)
    VKSTRUCT_PROPERTY(VkBool32, alphaToOneEnable)
END_DECLARE_VKSTRUCT()


BEGIN_DECLARE_UNTYPED_VKSTRUCT(PipelineColorBlendAttachmentState)
    VKSTRUCT_PROPERTY(VkBool32, blendEnable)
    VKSTRUCT_PROPERTY(VkBlendFactor, srcColorBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendFactor, dstColorBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendOp, colorBlendOp)
    VKSTRUCT_PROPERTY(VkBlendFactor, srcAlphaBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendFactor, dstAlphaBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendOp, alphaBlendOp)
    VKSTRUCT_PROPERTY(VkColorComponentFlags, colorWriteMask)
END_DECLARE_VKSTRUCT()


BEGIN_DECLARE_VKSTRUCT(PipelineColorBlendStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineColorBlendStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, logicOpEnable)
    VKSTRUCT_PROPERTY(VkLogicOp, logicOp)
    VKSTRUCT_PROPERTY(uint32_t, attachmentCount)
    VKSTRUCT_PROPERTY(const VkPipelineColorBlendAttachmentState*, pAttachments)
    VKSTRUCT_PROPERTY(std::array<float COMMA 4>, blendConstants)
END_DECLARE_VKSTRUCT()

inline constexpr VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
    const IsArrayContainer auto& colorBlendAttachments,
    VkBool32 logicOpEnable = VK_FALSE,
    VkLogicOp logicOp = VK_LOGIC_OP_COPY,
    const std::array<float, 4>& blendConstants = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f },
    VkPipelineColorBlendStateCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkPipelineColorBlendStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    result.logicOpEnable = logicOpEnable;
    result.logicOp = logicOp;
    result.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    result.pAttachments = colorBlendAttachments.data();
    result.blendConstants[0] = blendConstants[0];
    result.blendConstants[1] = blendConstants[1];
    result.blendConstants[2] = blendConstants[2];
    result.blendConstants[3] = blendConstants[3];

    return result;
}

BEGIN_DECLARE_VKSTRUCT(PipelineDepthStencilStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineDepthStencilStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, depthTestEnable)
    VKSTRUCT_PROPERTY(VkBool32, depthWriteEnable)
    VKSTRUCT_PROPERTY(VkCompareOp, depthCompareOp)
    VKSTRUCT_PROPERTY(VkBool32, depthBoundsTestEnable)
    VKSTRUCT_PROPERTY(VkBool32, stencilTestEnable)
    VKSTRUCT_PROPERTY(VkStencilOpState, front)
    VKSTRUCT_PROPERTY(VkStencilOpState, back)
    VKSTRUCT_PROPERTY(float, minDepthBounds)
    VKSTRUCT_PROPERTY(float, maxDepthBounds)
END_DECLARE_VKSTRUCT()

inline constexpr VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
    VkBool32 depthTestEnable,
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp,
    VkBool32 depthBoundsTestEnable,
    VkBool32 stencilTestEnable,
    VkStencilOpState front = {},
    VkStencilOpState back = {},
    float minDepthBounds = 0.0f,
    float maxDepthBounds = 0.0f,
    VkPipelineDepthStencilStateCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkPipelineDepthStencilStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    result.depthTestEnable = depthTestEnable;
    result.depthWriteEnable = depthWriteEnable;
    result.depthCompareOp = depthCompareOp;
    result.depthBoundsTestEnable = depthBoundsTestEnable;
    result.stencilTestEnable = stencilTestEnable;
    result.front = front;
    result.back = back;
    result.minDepthBounds = minDepthBounds;
    result.maxDepthBounds = maxDepthBounds;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(uint32_t maxSets,
    std::span<const VkDescriptorPoolSize> poolSizes,
    VkDescriptorPoolCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkDescriptorPoolCreateInfo result;
    result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    result.pNext = pNext;
    result.flags = flags;
    result.maxSets = maxSets;
    result.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    result.pPoolSizes = poolSizes.data();

    return result;
}

inline constexpr VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type,
    uint32_t descriptorCount)
{
    VkDescriptorPoolSize result{};
    result.type = type;
    result.descriptorCount = descriptorCount;

    return result;
}

inline constexpr VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
    VkDescriptorPool descriptorPool,
    const VkDescriptorSetLayout* pSetLayouts,
    uint32_t descriptorSetCount)
{
    VkDescriptorSetAllocateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    result.descriptorPool = descriptorPool;
    result.pSetLayouts = pSetLayouts;
    result.descriptorSetCount = descriptorSetCount;

    return result;
}

inline constexpr VkDescriptorBufferInfo descriptorBufferInfo(
    VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo result{};
    result.buffer = buffer;
    result.offset = offset;
    result.range = range;

    return result;
}

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorSetLayoutBinding)
    VKSTRUCT_PROPERTY(uint32_t, binding)
    VKSTRUCT_PROPERTY(VkDescriptorType, descriptorType)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
    VKSTRUCT_PROPERTY(VkShaderStageFlags, stageFlags)
    VKSTRUCT_PROPERTY(const VkSampler*, pImmutableSamplers)
END_DECLARE_VKSTRUCT()

inline constexpr VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectFlags,
    uint32_t baseMipLevel,
    uint32_t levelCount,
    uint32_t baseArrayLayer,
    uint32_t layerCount)
{
    VkImageSubresourceRange result{};
    result.aspectMask = aspectFlags;
    result.baseMipLevel = 0;
    result.levelCount = 1;
    result.baseArrayLayer = 0;
    result.layerCount = 1;

    return result;
}

inline constexpr VkExtent3D extent3D(uint32_t width, uint32_t height, uint32_t depth)
{
    VkExtent3D result{};
    result.width = width;
    result.height = height;
    result.depth = depth;

    return result;
}

inline constexpr VkImageCreateInfo imageCreateInfo(VkImageType type,
    VkExtent3D extent,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkFormat format,
    VkImageTiling tiling,
    VkImageLayout initialLayout,
    VkImageUsageFlags usage,
    VkSampleCountFlagBits samples,
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    std::span<const uint32_t> queueFamilyIndices = {},
    VkImageCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkImageCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    result.imageType = VK_IMAGE_TYPE_2D;
    result.extent = extent;
    result.mipLevels = mipLevels;
    result.arrayLayers = arrayLayers;
    result.format = format;
    result.tiling = tiling;
    result.initialLayout = initialLayout;
    result.usage = usage;
    result.samples = samples;
    result.sharingMode = sharingMode;
    result.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    result.pQueueFamilyIndices = queueFamilyIndices.data();
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

BEGIN_DECLARE_VKSTRUCT(PipelineShaderStageCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineShaderStageCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkShaderStageFlagBits, stage)
    VKSTRUCT_PROPERTY(VkShaderModule, module)
    VKSTRUCT_PROPERTY(const char*, pName)
    VKSTRUCT_PROPERTY(const VkSpecializationInfo*, pSpecializationInfo)
END_DECLARE_VKSTRUCT();

BEGIN_DECLARE_VKSTRUCT(PipelineVertexInputStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineVertexInputStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, vertexBindingDescriptionCount)
    VKSTRUCT_PROPERTY(const VkVertexInputBindingDescription*, pVertexBindingDescriptions)
    VKSTRUCT_PROPERTY(uint32_t, vertexAttributeDescriptionCount)
    VKSTRUCT_PROPERTY(const VkVertexInputAttributeDescription*, pVertexAttributeDescriptions)
END_DECLARE_VKSTRUCT();

inline constexpr VkFramebufferCreateInfo frameBufferCreateInfo(VkRenderPass renderPass,
    const IsArrayContainer auto& attachments,
    uint32_t width,
    uint32_t height,
    uint32_t layers,
    VkFramebufferCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkFramebufferCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    result.renderPass = renderPass;
    result.attachmentCount = static_cast<uint32_t>(attachments.size());
    result.pAttachments = attachments.data();
    result.width = width;
    result.height = height;
    result.layers = layers;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkSwapchainCreateInfoKHR swapChainCreateInfoKHR(VkSurfaceKHR surface,
    uint32_t minImageCount,
    VkFormat imageFormat,
    VkColorSpaceKHR imageColorSpace,
    VkExtent2D imageExtent,
    uint32_t imageArrayLayers,
    VkImageUsageFlags imageUsage,
    VkSharingMode imageSharingMode,
    std::span<const uint32_t> queueFamilyIndices,
    VkSurfaceTransformFlagBitsKHR preTransform,
    VkCompositeAlphaFlagBitsKHR compositeAlpha,
    VkPresentModeKHR presentMode,
    VkBool32 clipped,
    VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE,
    VkSwapchainCreateFlagsKHR flags = 0,
    const void* pNext = nullptr)
{
    VkSwapchainCreateInfoKHR result;
    result.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    result.surface = surface;
    result.minImageCount = minImageCount;
    result.imageFormat = imageFormat;
    result.imageColorSpace = imageColorSpace;
    result.imageExtent = imageExtent;
    result.imageArrayLayers = imageArrayLayers;
    result.imageUsage = imageUsage;
    result.imageSharingMode = imageSharingMode;
    result.queueFamilyIndexCount = queueFamilyIndices.size();
    result.pQueueFamilyIndices = queueFamilyIndices.data();
    result.preTransform = preTransform;
    result.compositeAlpha = compositeAlpha;
    result.presentMode = presentMode;
    result.clipped = clipped;
    result.oldSwapchain = oldSwapchain;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

}}    //  namespace vk::handles
