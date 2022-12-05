#pragma once

#include <cassert>

#include <vulkan/vulkan.h>

#include "utils.hpp"

namespace vk { namespace create {

inline VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;

    return bufferInfo;
}

inline VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex, const void* pNext = nullptr)
{
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    allocInfo.pNext = pNext;

    return allocInfo;
}

 inline VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool,
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

inline VkShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t codeSize, const uint32_t* codeData)
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

inline VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
            VkDescriptorPool descriptorPool, const VkDescriptorSetLayout* pSetLayouts, uint32_t descriptorSetCount)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
    descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;

    return descriptorSetAllocateInfo;
}

inline VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo bufferInfoDyn{};
    bufferInfoDyn.buffer = buffer;
    bufferInfoDyn.offset = offset;
    bufferInfoDyn.range = range;

    return bufferInfoDyn;
}

inline VkWriteDescriptorSet writeDescriptorSet(VkDescriptorSet descriptorSet, uint32_t dstBinding,
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

inline VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectFlags,
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

inline VkImageViewCreateInfo imageViewCreateInfo(VkImage image, VkImageViewType type, VkFormat format,
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

inline VkExtent3D extent3D(uint32_t width, uint32_t height, uint32_t depth)
{
    VkExtent3D extent{};
    extent.width = width;
    extent.height = height;
    extent.depth = depth;

    return extent;
}

inline VkImageCreateInfo imageCreateInfo(VkImageType type, VkExtent3D extent,
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

}
}
