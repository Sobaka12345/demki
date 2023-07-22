#pragma once

#include <array>
#include <concepts>

#include "utils.hpp"

namespace vk { namespace handles {

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

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorBufferInfo)
    VKSTRUCT_PROPERTY(VkBuffer, buffer)
    VKSTRUCT_PROPERTY(VkDeviceSize, offset)
    VKSTRUCT_PROPERTY(VkDeviceSize, range)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorImageInfo)
    VKSTRUCT_PROPERTY(VkSampler, sampler)
    VKSTRUCT_PROPERTY(VkImageView, imageView)
    VKSTRUCT_PROPERTY(VkImageLayout, imageLayout)
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

BEGIN_DECLARE_UNTYPED_VKSTRUCT(AttachmentDescription)
    VKSTRUCT_PROPERTY(VkAttachmentDescriptionFlags, flags)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, samples)
    VKSTRUCT_PROPERTY(VkAttachmentLoadOp, loadOp)
    VKSTRUCT_PROPERTY(VkAttachmentStoreOp, storeOp)
    VKSTRUCT_PROPERTY(VkAttachmentLoadOp, stencilLoadOp)
    VKSTRUCT_PROPERTY(VkAttachmentStoreOp, stencilStoreOp)
    VKSTRUCT_PROPERTY(VkImageLayout, initialLayout)
    VKSTRUCT_PROPERTY(VkImageLayout, finalLayout)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(AttachmentReference)
    VKSTRUCT_PROPERTY(uint32_t, attachment)
    VKSTRUCT_PROPERTY(VkImageLayout, layout)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(SubpassDescription)
    VKSTRUCT_PROPERTY(VkSubpassDescriptionFlags, flags)
    VKSTRUCT_PROPERTY(VkPipelineBindPoint, pipelineBindPoint)
    VKSTRUCT_PROPERTY(uint32_t, inputAttachmentCount)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pInputAttachments)
    VKSTRUCT_PROPERTY(uint32_t, colorAttachmentCount)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pColorAttachments)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pResolveAttachments)
    VKSTRUCT_PROPERTY(const VkAttachmentReference*, pDepthStencilAttachment)
    VKSTRUCT_PROPERTY(uint32_t, preserveAttachmentCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pPreserveAttachments)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(SubpassDependency)
    VKSTRUCT_PROPERTY(uint32_t, srcSubpass)
    VKSTRUCT_PROPERTY(uint32_t, dstSubpass)
    VKSTRUCT_PROPERTY(VkPipelineStageFlags, srcStageMask)
    VKSTRUCT_PROPERTY(VkPipelineStageFlags, dstStageMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, srcAccessMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, dstAccessMask)
    VKSTRUCT_PROPERTY(VkDependencyFlags, dependencyFlags)
END_DECLARE_VKSTRUCT()

}}    //  namespace vk::handles
