#pragma once

#include <array>
#include <cassert>
#include <concepts>

#include <vulkan/vulkan.h>

#include "utils.hpp"

namespace vk { namespace create {

template <typename T>
concept IsArrayContainer = requires (const T& o) {
    o.data();
    o.size();
};

inline constexpr VkBufferCopy bufferCopy(VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0)
{
    VkBufferCopy bufferCopy{};
    bufferCopy.size = size;
    bufferCopy.srcOffset = srcOffset;
    bufferCopy.dstOffset = dstOffset;

    return bufferCopy;
}

inline constexpr VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;

    return bufferInfo;
}

inline constexpr VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex, const void* pNext = nullptr)
{
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    allocInfo.pNext = pNext;

    return allocInfo;
}

 inline constexpr VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool,
    VkCommandBufferLevel level, uint32_t commandBufferCount, const void* pNext = nullptr)
 {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = level;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = commandBufferCount;
    allocInfo.pNext = pNext;

    return allocInfo;
 }

inline void buffer(VkDevice device, VkPhysicalDevice physicalDevice,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
   VkBufferCreateInfo bufferInfo = bufferCreateInfo(size, usage, VK_SHARING_MODE_EXCLUSIVE);
   assert(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS);

   VkMemoryRequirements memRequirements;
   vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

   VkMemoryAllocateInfo allocInfo = memoryAllocateInfo(
               memRequirements.size, utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties));
   assert(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) == VK_SUCCESS);

   vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

inline constexpr VkShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t codeSize, const uint32_t* codeData)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = codeData;

    return createInfo;
}

inline VkShaderModule shaderModule(VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo = shaderModuleCreateInfo(static_cast<uint32_t>(code.size()),
        reinterpret_cast<const uint32_t*>(code.data()));

    VkShaderModule shaderModule;
    assert(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS);

    return shaderModule;
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
    const IsArrayContainer auto& dynamicStates,
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
    const IsArrayContainer auto&    setLayouts,
    const IsArrayContainer auto&    pushConstantRanges,
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

inline constexpr VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = type;
    descriptorPoolSize.descriptorCount = descriptorCount;

    return descriptorPoolSize;
}

inline constexpr VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
            VkDescriptorPool descriptorPool, const VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
    descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;

    return descriptorSetAllocateInfo;
}

inline constexpr VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo bufferInfoDyn{};
    bufferInfoDyn.buffer = buffer;
    bufferInfoDyn.offset = offset;
    bufferInfoDyn.range = range;

    return bufferInfoDyn;
}

inline constexpr VkWriteDescriptorSet writeDescriptorSet(VkDescriptorSet descriptorSet, uint32_t dstBinding,
    VkDescriptorType descriptorType, uint32_t descriptorCount, VkDescriptorBufferInfo* bufferInfo, uint32_t dstArrayElement = 0)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = dstBinding;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.descriptorCount = descriptorCount;
    descriptorWrite.pBufferInfo = bufferInfo;
    descriptorWrite.dstArrayElement = dstArrayElement;

    return descriptorWrite;
}

inline constexpr VkDescriptorSetLayoutBinding setLayoutBinding(
    VkDescriptorType descriptorType, VkShaderStageFlags shaderStageFlags,
    uint32_t binding, uint32_t descriptorCount = 1, VkSampler* pImmutableSamplers = nullptr)
{
    VkDescriptorSetLayoutBinding result{};
    result.binding = binding;
    result.descriptorCount = descriptorCount;
    result.descriptorType = descriptorType;
    result.pImmutableSamplers = pImmutableSamplers;
    result.stageFlags = shaderStageFlags;

    return result;
}

inline constexpr VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectFlags,
    uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
{
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = aspectFlags;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return subresourceRange;
}

inline constexpr VkImageViewCreateInfo imageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format,
    VkImageSubresourceRange subresourceRange, VkComponentMapping components = {}, const void* pNext = nullptr)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = type;
    viewInfo.format = format;
    viewInfo.subresourceRange = subresourceRange;
    viewInfo.components = components;
    viewInfo.pNext = pNext;

    return viewInfo;
}

inline constexpr VkExtent3D extent3D(uint32_t width, uint32_t height, uint32_t depth)
{
    VkExtent3D extent{};
    extent.width = width;
    extent.height = height;
    extent.depth = depth;

    return extent;
}

inline constexpr VkImageCreateInfo imageCreateInfo(VkImageType type, VkExtent3D extent,
    uint32_t mipLevels, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
    VkImageLayout initialLayout, VkImageUsageFlags usage, VkSampleCountFlagBits samples,
    VkSharingMode sharingMode)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = samples;
    imageInfo.sharingMode = sharingMode;

    return imageInfo;
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
    const IsArrayContainer auto& stages = {},
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
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
    pipelineInfo.pStages = stages.data();
    pipelineInfo.pVertexInputState = pVertexInputState;
    pipelineInfo.pInputAssemblyState = pInputAssemblyState;
    pipelineInfo.pViewportState = pViewportState;
    pipelineInfo.pRasterizationState = pRasterizationState;
    pipelineInfo.pMultisampleState = pMultisampleState;
    pipelineInfo.pDepthStencilState = pDepthStencilState;
    pipelineInfo.pColorBlendState = pColorBlendState;
    pipelineInfo.pDynamicState = pDynamicState;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = basePipelineHandle;
    pipelineInfo.basePipelineIndex = basePipelineIndex;

    return pipelineInfo;
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

}
}
