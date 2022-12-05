#pragma once

#include <cassert>

#include <vulkan/vulkan.h>

#include "Utils.hpp"

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

}
}
