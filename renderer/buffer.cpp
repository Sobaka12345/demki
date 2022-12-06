#include "buffer.hpp"

#include "creators.hpp"

namespace vk {

Buffer::Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode)
    : m_device(device)
    , m_size(size)
{
    VkBufferCreateInfo bufferInfo = create::bufferCreateInfo(size, usage, sharingMode);
    assert(vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) == VK_SUCCESS);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_device, m_buffer, nullptr);
    for(auto memory: m_memoryList)
    {
        vkFreeMemory(m_device, memory, nullptr);
    }
}

VkDeviceMemory Buffer::allocateMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, uint32_t bindingOffset)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = create::memoryAllocateInfo(
                memRequirements.size, utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties));

    m_memoryList.push_back(VkDeviceMemory{});
    assert(vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memoryList.back()) == VK_SUCCESS);

    vkBindBufferMemory(m_device, m_buffer, m_memoryList.back(), bindingOffset);
}

}