#include "buffer.hpp"

#include "device.hpp"
#include "image.hpp"
#include "memory.hpp"

namespace renderer::vk { namespace handles {

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

Buffer::Buffer(const Device& device, BufferCreateInfo bufferInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
    , m_size(bufferInfo.size())
{
    ASSERT(create(vkCreateBuffer, device, &bufferInfo, nullptr) == VK_SUCCESS);
}

Buffer::Buffer(const Device& device, BufferCreateInfo bufferInfo) noexcept
    : Buffer(device, std::move(bufferInfo), nullptr)
{}

Buffer::~Buffer()
{
    destroy(vkDestroyBuffer, m_device, handle(), nullptr);
}

bool Buffer::bindMemory(uint32_t bindingOffset)
{
    DASSERT(m_memory);
    return m_memory->bindBuffer(*this, bindingOffset);
}

void Buffer::copyTo(const Buffer& dst, VkBufferCopy copyRegion) const
{
    m_device.oneTimeCommand(GRAPHICS_COMPUTE)().copyBuffer(handle(), dst, { &copyRegion, 1 });
}

void Buffer::copyToImage(
    const Image& dst, VkImageLayout dstLayout, VkBufferImageCopy copyRegion) const
{
    m_device.oneTimeCommand(GRAPHICS_COMPUTE)().copyBufferToImage(handle(), dst, dstLayout,
        { &copyRegion, 1 });
}

std::weak_ptr<Memory> Buffer::allocateMemoryImpl(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, handle(), &memRequirements);

    m_memory = std::make_shared<Memory>(m_device,
        MemoryAllocateInfo{}
            .allocationSize(memRequirements.size)
            .memoryTypeIndex(findMemoryType(m_device.physicalDevice(),
                memRequirements.memoryTypeBits, properties)));

    return m_memory;
}

}}    //  namespace renderer::vk::handles
