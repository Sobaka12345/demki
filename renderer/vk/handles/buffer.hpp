#pragma once

#include "simemory_accessor.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

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
public:
    struct Descriptor
    {
        const Buffer& buffer;
        VkDeviceSize offset;
        VkDeviceSize range;
        size_t objectSize;

        size_t objectCount() const { return range / objectSize; }
    };

public:
    static BufferCreateInfo staging();

    Buffer(Buffer&& other) noexcept;
    Buffer(const Device& device, VkHandleType* bufferInfo) noexcept;
    Buffer(const Device& device, BufferCreateInfo bufferInfo, VkHandleType* handlePtr = nullptr);
    ~Buffer();

    bool bindMemory(uint32_t bindingOffset);

    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
    {
        return allocateMemoryImpl(properties);
    }

    void copyTo(const Buffer& dst, VkBufferCopy copyRegion) const;
    void copyToImage(const Image& dst, VkImageLayout dstLayout, VkBufferImageCopy copyRegion) const;

    VkDeviceSize size() const { return m_size; }

private:
    std::weak_ptr<Memory> allocateMemoryImpl(VkMemoryPropertyFlags properties);

private:
    VkDeviceSize m_size;
};

}}    //  namespace vk::handles
