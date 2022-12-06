#include "buffer.hpp"

#include "creators.hpp"

namespace vk {

Buffer::Memory::Memory(const Buffer& buffer, VkMemoryAllocateInfo allocInfo)
    : buffer(buffer)
    , size(allocInfo.allocationSize)
    , memoryType(allocInfo.memoryTypeIndex)
{
    assert(vkAllocateMemory(buffer.device(), &allocInfo, nullptr, &deviceMemory) == VK_SUCCESS);
}

Buffer::Memory::~Memory()
{
    vkFreeMemory(buffer.device(), deviceMemory, nullptr);
}

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
}

bool Buffer::bindMemory(uint32_t bindingOffset)
{
    return vkBindBufferMemory(m_device, m_buffer, m_memory->deviceMemory, bindingOffset) == VK_SUCCESS;
}

const Buffer::Memory& Buffer::allocateMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    m_memory = std::make_unique<Memory>(*this, create::memoryAllocateInfo(
        memRequirements.size,
        utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties))
    );

    return *m_memory;
}

const Buffer::Memory& Buffer::allocateAndBindMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, uint32_t bindingOffset)
{
    allocateMemory(physicalDevice, properties);
    assert(bindMemory(bindingOffset));
    return *m_memory;
}

bool Buffer::mapMemory(void** data, VkMemoryMapFlags flags, VkDeviceSize offset)
{
    return vkMapMemory(m_device, m_memory->deviceMemory, 0, size(), 0, data) == VK_SUCCESS;
}

void Buffer::unmapMemory()
{
    vkUnmapMemory(m_device, m_memory->deviceMemory);
}

void Buffer::copyTo(const Buffer& buffer, VkCommandPool commandPool, VkQueue queue, VkBufferCopy copyRegion)
{
    const auto allocInfo =
        create::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    VkCommandBuffer commandBuffer;
    assert(vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer) == VK_SUCCESS);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    assert(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);
    vkCmdCopyBuffer(commandBuffer, m_buffer, buffer.m_buffer, 1, &copyRegion);
    assert(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    assert(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS);
    assert(vkQueueWaitIdle(queue) == VK_SUCCESS);

    vkFreeCommandBuffers(m_device, commandPool, 1, &commandBuffer);
}

}