#pragma once

#include "creators.hpp"

#include <vulkan/vulkan.h>

namespace vk {

class Buffer
{
    Buffer(VkDevice device, VkPhysicalDevice physicalDevice,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
   VkBufferCreateInfo bufferInfo = create::bufferCreateInfo(size, usage, VK_SHARING_MODE_EXCLUSIVE);
   assert(vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) == VK_SUCCESS);

   VkMemoryRequirements memRequirements;
   vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

   VkMemoryAllocateInfo allocInfo = create::memoryAllocateInfo(
               memRequirements.size, utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties));
   assert(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) == VK_SUCCESS);

   vkBindBufferMemory(device, m_buffer, bufferMemory, 0);
}

    VkBuffer m_buffer;
    VkDeviceMemory bufferMemory;
};

}