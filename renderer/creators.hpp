#pragma once

#include <array>
#include <concepts>

#include "utils.hpp"

namespace vk { namespace create {

template <typename T>
concept IsArrayContainer = requires (const T& o) {
    o.data();
    o.size();
};




inline constexpr VkSubmitInfo submitInfo(
    std::span<const VkCommandBuffer> commandBuffers,
    std::span<const VkSemaphore> waitSemaphores = {},
    const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
    std::span<const VkSemaphore> signalSemaphores = {},
    const void*                 pNext = nullptr)
{
    VkSubmitInfo result;

    result.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    result.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    result.pCommandBuffers = commandBuffers.data();
    result.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    result.pWaitSemaphores = waitSemaphores.data();
    result.pWaitDstStageMask = pWaitDstStageMask;
    result.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    result.pSignalSemaphores = signalSemaphores.data();
    result.pNext = pNext;

    return result;
}

inline VkDeviceQueueCreateInfo deviceQueueCreateInfo(
    uint32_t                    queueFamilyIndex,
    const std::span<const float>      queuePriorities,
    VkDeviceQueueCreateFlags    flags = 0,
    const void*                 pNext = nullptr)
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
    const VkPhysicalDeviceFeatures*    pEnabledFeatures = nullptr,
    VkDeviceCreateFlags                flags = 0,
    const void*                        pNext = nullptr)
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

inline constexpr VkBufferCopy bufferCopy(VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0)
{
    VkBufferCopy result{};
    result.size = size;
    result.srcOffset = srcOffset;
    result.dstOffset = dstOffset;

    return result;
}

inline constexpr VkImageSubresourceLayers imageSubresourceLayers(
    VkImageAspectFlags    aspectMask,
    uint32_t              mipLevel,
    uint32_t              baseArrayLayer,
    uint32_t              layerCount)
{
    VkImageSubresourceLayers result;
    result.aspectMask = aspectMask;
    result.mipLevel = mipLevel;
    result.baseArrayLayer = baseArrayLayer;
    result.layerCount = layerCount;

    return result;
}

inline constexpr VkBufferImageCopy bufferImageCopy(
    VkDeviceSize                bufferOffset,
    uint32_t                    bufferRowLength,
    uint32_t                    bufferImageHeight,
    VkImageSubresourceLayers    imageSubresource,
    VkOffset3D                  imageOffset,
    VkExtent3D                  imageExtent)
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

inline constexpr VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage,
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE)
{
    VkBufferCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    result.size = size;
    result.usage = usage;
    result.sharingMode = sharingMode;

    return result;
}

inline constexpr VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex, const void* pNext = nullptr)
{
    VkMemoryAllocateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    result.allocationSize = size;
    result.memoryTypeIndex = memoryTypeIndex;
    result.pNext = pNext;

    return result;
}

inline constexpr VkMappedMemoryRange mappedMemoryRange(
    VkDeviceMemory     memory,
    VkDeviceSize       offset,
    VkDeviceSize       size,
    const void*        pNext = nullptr)
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
    uint32_t                    queueFamilyIndex,
    VkCommandPoolCreateFlags    flags,
    const void*                 pNext = nullptr
)
{
    VkCommandPoolCreateInfo result;
    result.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    result.queueFamilyIndex = queueFamilyIndex;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

 inline constexpr VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool,
    VkCommandBufferLevel level, uint32_t commandBufferCount, const void* pNext = nullptr)
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
    VkCommandBufferUsageFlags             flags = 0,
    const void*                           pNext = nullptr)
 {
     VkCommandBufferBeginInfo result;
     result.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
     result.pInheritanceInfo = pInheritanceInfo;
     result.flags = flags;
     result.pNext = pNext;

     return result;
 }

 inline constexpr VkRenderPassBeginInfo renderPassBeginInfo(
     VkRenderPass           renderPass,
     VkFramebuffer          framebuffer,
     VkRect2D               renderArea,
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

inline constexpr VkShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t codeSize, const uint32_t* codeData)
{
    VkShaderModuleCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    result.codeSize = codeSize;
    result.pCode = codeData;

    return result;
}

inline VkShaderModule shaderModule(VkDevice device, const std::vector<char> &code)
{
    const VkShaderModuleCreateInfo createInfo = shaderModuleCreateInfo(static_cast<uint32_t>(code.size()),
        reinterpret_cast<const uint32_t*>(code.data()));

    VkShaderModule result = VK_NULL_HANDLE;
    ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &result) == VK_SUCCESS);

    return result;
}

inline constexpr VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
    const IsArrayContainer auto& viewports,
    const IsArrayContainer auto& scissors,
    VkPipelineViewportStateCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkPipelineViewportStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    result.viewportCount = static_cast<uint32_t>(viewports.size());
    result.pViewports = viewports.data();
    result.scissorCount = static_cast<uint32_t>(scissors.size());
    result.pScissors = scissors.data();

    return result;
}

inline constexpr VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
    std::span<const VkDynamicState> dynamicStates,
    VkPipelineDynamicStateCreateFlags flags = 0, const void* pNext = nullptr)
{
    VkPipelineDynamicStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    result.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    result.pDynamicStates = dynamicStates.data();
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
    VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable,
    VkPipelineInputAssemblyStateCreateFlags flags = 0,
    const void* pNext = nullptr)
{
    VkPipelineInputAssemblyStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    result.topology = topology;
    result.primitiveRestartEnable = primitiveRestartEnable;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
    VkBool32                                   depthClampEnable,
    VkBool32                                   rasterizerDiscardEnable,
    VkPolygonMode                              polygonMode,
    VkCullModeFlags                            cullMode,
    VkFrontFace                                frontFace,
    VkBool32                                   depthBiasEnable,
    float                                      lineWidth = 1.0f,
    float                                      depthBiasConstantFactor = 0.0f,
    float                                      depthBiasClamp = 0.0f,
    float                                      depthBiasSlopeFactor = 0.0f,
    VkPipelineRasterizationStateCreateFlags    flags = 0,
    const void*                                pNext = nullptr)
{
    VkPipelineRasterizationStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    result.depthClampEnable = depthClampEnable;
    result.rasterizerDiscardEnable = rasterizerDiscardEnable;
    result.polygonMode = polygonMode;
    result.lineWidth = lineWidth;
    result.cullMode = cullMode;
    result.frontFace = frontFace;
    result.depthBiasEnable = depthBiasEnable;
    result.depthBiasConstantFactor = depthBiasConstantFactor;
    result.depthBiasClamp = depthBiasClamp;
    result.depthBiasSlopeFactor = depthBiasSlopeFactor;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
    VkBool32                                 sampleShadingEnable,
    VkSampleCountFlagBits                    rasterizationSamples,
    float                                    minSampleShading = 1.0f,
    const VkSampleMask*                      pSampleMask = nullptr,
    VkBool32                                 alphaToCoverageEnable = VK_FALSE,
    VkBool32                                 alphaToOneEnable = VK_FALSE,
    VkPipelineMultisampleStateCreateFlags    flags = 0,
    const void*                              pNext = nullptr)
{
    VkPipelineMultisampleStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    result.sampleShadingEnable = sampleShadingEnable;
    result.rasterizationSamples = rasterizationSamples;
    result.minSampleShading = minSampleShading;
    result.pSampleMask = pSampleMask;
    result.alphaToCoverageEnable = alphaToCoverageEnable;
    result.alphaToOneEnable = alphaToOneEnable;
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
    VkColorComponentFlags    colorWriteMask,
    VkBool32                 blendEnable = VK_FALSE,
    VkBlendFactor            srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
    VkBlendFactor            dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    VkBlendOp                colorBlendOp = VK_BLEND_OP_ADD,
    VkBlendFactor            srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    VkBlendFactor            dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    VkBlendOp                alphaBlendOp = VK_BLEND_OP_ADD)
{
    VkPipelineColorBlendAttachmentState result{};
    result.colorWriteMask = colorWriteMask;
    result.blendEnable = blendEnable;
    result.srcColorBlendFactor = srcColorBlendFactor;
    result.dstColorBlendFactor = dstColorBlendFactor;
    result.colorBlendOp = colorBlendOp;
    result.srcAlphaBlendFactor = srcAlphaBlendFactor;
    result.dstAlphaBlendFactor = dstAlphaBlendFactor;
    result.alphaBlendOp = alphaBlendOp;

    return result;
}

inline constexpr VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
    const IsArrayContainer auto&                  colorBlendAttachments,
    VkBool32                                      logicOpEnable = VK_FALSE,
    VkLogicOp                                     logicOp = VK_LOGIC_OP_COPY,
    const std::array<float, 4>&                   blendConstants = std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f},
    VkPipelineColorBlendStateCreateFlags          flags = 0,
    const void*                                   pNext = nullptr)
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

inline constexpr VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
    VkBool32                                  depthTestEnable,
    VkBool32                                  depthWriteEnable,
    VkCompareOp                               depthCompareOp,
    VkBool32                                  depthBoundsTestEnable,
    VkBool32                                  stencilTestEnable,
    VkStencilOpState                          front = {},
    VkStencilOpState                          back = {},
    float                                     minDepthBounds = 0.0f,
    float                                     maxDepthBounds = 0.0f,
    VkPipelineDepthStencilStateCreateFlags    flags = 0,
    const void*                               pNext = nullptr)
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

inline constexpr VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
    std::span<const VkDescriptorSetLayout>    setLayouts,
    std::span<const VkPushConstantRange>    pushConstantRanges,
    VkPipelineLayoutCreateFlags     flags = 0,
    const void*                     pNext = nullptr)
{
    VkPipelineLayoutCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    result.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    result.pSetLayouts = setLayouts.data();
    result.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    result.pPushConstantRanges = pushConstantRanges.data();
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(
    uint32_t                       maxSets,
    std::span<const VkDescriptorPoolSize> poolSizes,
    VkDescriptorPoolCreateFlags    flags = 0,
    const void*                    pNext = nullptr)
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

inline constexpr VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
    VkDescriptorPoolSize result{};
    result.type = type;
    result.descriptorCount = descriptorCount;

    return result;
}

inline constexpr VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
    VkDescriptorPool descriptorPool, const VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount)
{
    VkDescriptorSetAllocateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    result.descriptorPool = descriptorPool;
    result.pSetLayouts = pSetLayouts;
    result.descriptorSetCount = descriptorSetCount;

    return result;
}

inline constexpr VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo result{};
    result.buffer = buffer;
    result.offset = offset;
    result.range = range;

    return result;
}

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorSetLayoutBinding)
VKSTRUCT_PROPERTY(uint32_t          , binding)
VKSTRUCT_PROPERTY(VkDescriptorType  , descriptorType)
VKSTRUCT_PROPERTY(uint32_t          , descriptorCount)
VKSTRUCT_PROPERTY(VkShaderStageFlags, stageFlags)
VKSTRUCT_PROPERTY(const VkSampler*  , pImmutableSamplers)
END_DECLARE_VKSTRUCT()



inline constexpr VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectFlags,
    uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
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

inline constexpr VkImageCreateInfo imageCreateInfo(VkImageType type, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
    VkImageLayout initialLayout, VkImageUsageFlags usage, VkSampleCountFlagBits samples,
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    std::span<const uint32_t> queueFamilyIndices = {},
    VkImageCreateFlags flags = 0, const void* pNext = nullptr)
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

inline constexpr VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(
    VkShaderStageFlagBits stage, VkShaderModule module, const char* pName = "main",
    VkPipelineShaderStageCreateFlags flags = 0, const VkSpecializationInfo* pSpecializationInfo = nullptr,
    const void* pNext = nullptr)
{
    VkPipelineShaderStageCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    result.stage = stage;
    result.pName = pName;
    result.flags = flags;
    result.module = module;
    result.pNext = pNext;
    result.pSpecializationInfo = pSpecializationInfo;

    return result;
}

inline constexpr VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
    VkPipelineLayout layout, VkRenderPass renderPass, VkPipelineCreateFlags flags,
    std::span<const VkPipelineShaderStageCreateInfo> stages = {},
    const VkPipelineVertexInputStateCreateInfo* pVertexInputState = nullptr,
    const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState = nullptr,
    const VkPipelineTessellationStateCreateInfo* pTessellationState = nullptr,
    const VkPipelineViewportStateCreateInfo* pViewportState = nullptr,
    const VkPipelineRasterizationStateCreateInfo* pRasterizationState = nullptr,
    const VkPipelineMultisampleStateCreateInfo* pMultisampleState = nullptr,
    const VkPipelineColorBlendStateCreateInfo* pColorBlendState = nullptr,
    const VkPipelineDynamicStateCreateInfo* pDynamicState = nullptr,
    const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState = nullptr,
    VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = -1,
    uint32_t subpass = 0, const void* pNext = nullptr)
{
    VkGraphicsPipelineCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    result.stageCount = static_cast<uint32_t>(stages.size());
    result.pStages = stages.data();
    result.pVertexInputState = pVertexInputState;
    result.pInputAssemblyState = pInputAssemblyState;
    result.pViewportState = pViewportState;
    result.pRasterizationState = pRasterizationState;
    result.pMultisampleState = pMultisampleState;
    result.pDepthStencilState = pDepthStencilState;
    result.pColorBlendState = pColorBlendState;
    result.pDynamicState = pDynamicState;
    result.layout = layout;
    result.renderPass = renderPass;
    result.subpass = subpass;
    result.basePipelineHandle = basePipelineHandle;
    result.basePipelineIndex = basePipelineIndex;

    return result;
}

inline constexpr VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
    const IsArrayContainer auto& vertexBindingDescriptions,
    const IsArrayContainer auto& vertexAttributeDescriptions,
    VkPipelineVertexInputStateCreateFlags flags = 0, const void* pNext = nullptr)
{
    VkPipelineVertexInputStateCreateInfo result{};
    result.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    result.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
    result.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
    result.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
    result.pVertexBindingDescriptions = vertexBindingDescriptions.data();
    result.flags = flags;
    result.pNext = pNext;

    return result;
}

inline constexpr VkFramebufferCreateInfo frameBufferCreateInfo(
    VkRenderPass renderPass,
    const IsArrayContainer auto& attachments,
    uint32_t width, uint32_t height,
    uint32_t layers, VkFramebufferCreateFlags flags = 0,
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

inline constexpr VkSwapchainCreateInfoKHR swapChainCreateInfoKHR(
    VkSurfaceKHR                     surface,
    uint32_t                         minImageCount,
    VkFormat                         imageFormat,
    VkColorSpaceKHR                  imageColorSpace,
    VkExtent2D                       imageExtent,
    uint32_t                         imageArrayLayers,
    VkImageUsageFlags                imageUsage,
    VkSharingMode                    imageSharingMode,
    std::span<const uint32_t>        queueFamilyIndices,
    VkSurfaceTransformFlagBitsKHR    preTransform,
    VkCompositeAlphaFlagBitsKHR      compositeAlpha,
    VkPresentModeKHR                 presentMode,
    VkBool32                         clipped,
    VkSwapchainKHR                   oldSwapchain = VK_NULL_HANDLE,
    VkSwapchainCreateFlagsKHR        flags = 0,
    const void*                      pNext = nullptr)
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
    result.queueFamilyIndexCount =  queueFamilyIndices.size();
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

}
}
