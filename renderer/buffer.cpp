#include "buffer.hpp"

#include "creators.hpp"
#include <iostream>

namespace vk {

Buffer::Memory::Mapped::Mapped(const Memory& memory, VkMemoryMapFlags flags, VkDeviceSize offset)
    : memory(memory)
    , offset(offset)
{
    assert(vkMapMemory(memory.buffer.device(), memory.deviceMemory,
        offset, memory.size, flags, &data) == VK_SUCCESS);
}

Buffer::Memory::Mapped::~Mapped()
{
    vkUnmapMemory(memory.buffer.device(), memory.deviceMemory);
}

void Buffer::Memory::Mapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    std::memcpy(
        reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + offset),
        src, static_cast<size_t>(size));
}

void Buffer::Memory::Mapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    VkMappedMemoryRange memoryRange{};
    memoryRange.memory = memory.deviceMemory;
    memoryRange.size = size;
    memoryRange.offset = static_cast<VkDeviceSize>(offset);
    vkFlushMappedMemoryRanges(memory.buffer.device(), 1, &memoryRange);
}

void Buffer::Memory::Mapped::writeAndSync(const void *src, VkDeviceSize size, ptrdiff_t offset)
{
    write(src, size, offset);
    sync(size, offset);
}

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

std::shared_ptr<Buffer::Memory::Mapped> Buffer::Memory::map(VkMemoryMapFlags flags, VkDeviceSize offset)
{
    assert(!mapped);
    mapped = std::make_shared<Mapped>(*this, flags, offset);

    return mapped;
}

void Buffer::Memory::unmap()
{
    if (mapped)
    {
        mapped.reset();
    }
}

Buffer::Buffer(Buffer&& other)
    : m_buffer(std::move(other.m_buffer))
    , m_device(std::move(other.m_device))
    , m_size(std::move(other.m_size))
    , m_memory(std::move(other.m_memory))
{
    other.m_buffer = VK_NULL_HANDLE;
}

Buffer::Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode)
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

std::shared_ptr<Buffer::Memory> Buffer::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    m_memory = std::make_shared<Memory>(*this, create::memoryAllocateInfo(
        memRequirements.size,
        utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits, properties))
    );

    return m_memory;
}

std::shared_ptr<Buffer::Memory> Buffer::allocateAndBindMemory(VkMemoryPropertyFlags properties, uint32_t bindingOffset)
{
    allocateMemory(properties);
    assert(bindMemory(bindingOffset));
    return m_memory;
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
