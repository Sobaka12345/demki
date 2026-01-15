#include "specific_buffer.hpp"

#include "handles/buffer.hpp"
#include "handles/memory.hpp"

#include "graphics_context.hpp"

namespace renderer::vk {

using namespace handles;

void ISpecificBuffer::allocateBuffer(
    size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    m_dynamicSize = m_size = size;
    m_buffer = Buffer::create(m_context.device(),
        BufferCreateInfo{}.size(size).usage(usage).sharingMode(VK_SHARING_MODE_EXCLUSIVE));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_context.device(), m_buffer, &memRequirements);

    m_alignment = memRequirements.alignment;
    m_bufferMemory = DeviceMemory::create(m_context.device(),
        MemoryAllocateInfo{}
            .allocationSize(memRequirements.size)
            .memoryTypeIndex(DeviceMemory::findMemoryType(m_context.physicalDevice(),
                memRequirements.memoryTypeBits, properties)));

    ASSERT(vkBindBufferMemory(m_context.device(), m_buffer, m_bufferMemory, 0) == VK_SUCCESS);
}

void ISpecificBuffer::destroy()
{
    DeviceMemory::destroy(m_context.device(), m_bufferMemory);
    Buffer::destroy(m_context.device(), m_buffer);
}

void ISpecificBuffer::setActiveRange(size_t size, size_t offset)
{
    m_dynamicSize = size;
    m_dynamicOffset = offset;
}

}    //  namespace renderer::vk
