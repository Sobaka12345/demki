#include "buffer.hpp"
#include "creators.hpp"
#include "image.hpp"

#include <iostream>

namespace vk { namespace handles {

BufferCreateInfo Buffer::staging()
{
    return BufferCreateInfo()
        .usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

Buffer::Buffer(Buffer&& other) noexcept
    : Handle(std::move(other))
    , SIMemoryAccessor(std::move(other))
    , m_size(std::move(other.m_size))
{}

Buffer::Buffer(const Device& device, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
    , m_size(0)
{}

Buffer::Buffer(const Device& device, BufferCreateInfo bufferInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
    , m_size(bufferInfo.size())
{
    ASSERT(create(vkCreateBuffer, device, &bufferInfo, nullptr) == VK_SUCCESS);
}

Buffer::~Buffer()
{
    destroy(vkDestroyBuffer, m_device, handle(), nullptr);
}

bool Buffer::bindMemory(uint32_t bindingOffset)
{
    DASSERT(m_memory);
    return vkBindBufferMemory(m_device, handle(), *m_memory, bindingOffset) == VK_SUCCESS;
}

std::shared_ptr<Memory> Buffer::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, handle(), &memRequirements);

    m_memory = std::make_shared<Memory>(m_device,
        memoryAllocateInfo(memRequirements.size,
            utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits,
                properties)));

    return m_memory;
}

void Buffer::copyTo(const Buffer& dst, VkBufferCopy copyRegion) const
{
    m_device.oneTimeCommand(GRAPHICS)().copyBuffer(handle(), dst, { &copyRegion, 1 });
}

void Buffer::copyToImage(
    const Image& dst, VkImageLayout dstLayout, VkBufferImageCopy copyRegion) const
{
    m_device.oneTimeCommand(GRAPHICS)().copyBufferToImage(handle(), dst, dstLayout,
        { &copyRegion, 1 });
}

}}    //  namespace vk::handles