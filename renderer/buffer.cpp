#include "buffer.hpp"

#include "creators.hpp"
#include <iostream>

namespace vk {

Buffer::Buffer(Buffer&& other)
    : SIMemoryAccessor(std::move(other))
    , m_size(std::move(other.m_size))
    , m_buffer(std::move(other.m_buffer))
{
    other.m_buffer = VK_NULL_HANDLE;
}

Buffer::Buffer(const Device& device, VkBufferCreateInfo bufferInfo)
    : SIMemoryAccessor(device, bufferInfo.sharingMode)
    , m_size(bufferInfo.size)
{
    ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) == VK_SUCCESS);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_device, m_buffer, nullptr);
}

bool Buffer::bindMemory(uint32_t bindingOffset)
{
    return vkBindBufferMemory(m_device, m_buffer, m_memory->deviceMemory, bindingOffset) == VK_SUCCESS;
}

std::shared_ptr<Memory> Buffer::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    m_memory = std::make_shared<Memory>(m_device, create::memoryAllocateInfo(
        memRequirements.size,
        utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits, properties))
    );

    return m_memory;
}

void Buffer::copyTo(const Buffer& buffer, VkCommandPool commandPool, VkQueue queue, VkBufferCopy copyRegion)
{
    const auto allocInfo =
        create::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    VkCommandBuffer commandBuffer;
    ASSERT(vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer) == VK_SUCCESS);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);
    vkCmdCopyBuffer(commandBuffer, m_buffer, buffer.m_buffer, 1, &copyRegion);
    ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    ASSERT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS);
    ASSERT(vkQueueWaitIdle(queue) == VK_SUCCESS);

    vkFreeCommandBuffers(m_device, commandPool, 1, &commandBuffer);
}

}
