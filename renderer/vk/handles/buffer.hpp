#pragma once

#include <memory>
#include "handle.hpp"

#include "simemory_accessor.hpp"

#include "vk/utils.hpp"

namespace renderer::vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(BufferCreateInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkBufferCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkDeviceSize, size)
    VKSTRUCT_PROPERTY(VkBufferUsageFlags, usage)
    VKSTRUCT_PROPERTY(VkSharingMode, sharingMode)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndexCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pQueueFamilyIndices)
END_DECLARE_VKSTRUCT()

class Image;
class Device;

class Buffer
    : public Handle<VkBuffer>
    , public SIMemoryAccessor<Buffer>
{
    HANDLE(Buffer);

public:
    static BufferCreateInfo staging();

    Buffer(Buffer&& other) noexcept;
    Buffer(const Device& device, BufferCreateInfo bufferInfo) noexcept;
    ~Buffer();

    bool bindMemory(uint32_t bindingOffset);

    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
    {
        return allocateMemoryImpl(properties);
    }

    void copyTo(const Buffer& dst, VkBufferCopy copyRegion) const;
    void copyToImage(const Image& dst, VkImageLayout dstLayout, VkBufferImageCopy copyRegion) const;

    VkDeviceSize size() const { return m_size; }

protected:
    Buffer(const Device& device, BufferCreateInfo bufferInfo, VkHandleType* handlePtr) noexcept;

private:
    std::weak_ptr<Memory> allocateMemoryImpl(VkMemoryPropertyFlags properties);

private:
    VkDeviceSize m_size;
};

}}    //  namespace renderer::vk::handles
