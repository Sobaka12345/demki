#pragma once

#include "utils.hpp"
#include "device.hpp"
#include "memory.hpp"

#include <list>
#include <span>
#include <memory>
#include <optional>

namespace vk {

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

class Buffer : public Handle<VkBuffer>, public SIMemoryAccessor<Buffer>
{
public:
    struct Descriptor
    {
        const Buffer& buffer;
        VkDeviceSize offset;
        VkDeviceSize range;
        size_t objectSize;

        size_t objectCount() const { return range / objectSize;  }
    };

public:
    static BufferCreateInfo staging();

    Buffer(Buffer&& other) noexcept;
    Buffer(const Device& device, VkHandleType* bufferInfo) noexcept;
    Buffer(const Device& device, BufferCreateInfo bufferInfo, VkHandleType* handlePtr = nullptr);
    ~Buffer();

    bool bindMemory(uint32_t bindingOffset);
    std::shared_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties);

    void copyTo(const Buffer& dst, VkBufferCopy copyRegion) const;
    void copyToImage(const Image& dst, VkImageLayout dstLayout, VkBufferImageCopy copyRegion) const;

    VkDeviceSize size() const { return m_size; }

private:
    VkDeviceSize m_size;
};
//
//class StagingBuffer: public Buffer
//{
//public:
//    StagingBuffer(const Device& device, VkDeviceSize size)
//        : Buffer(device, create::bufferCreateInfo(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE))
//    {}
//};
//
//class VertexBuffer: public Buffer
//{
//public:
//    VertexBuffer(const Device& device, size_t vertexCount, size_t vertexSize)
//        : Buffer(device, create::bufferCreateInfo(
//            static_cast<VkDeviceSize>(vertexCount * vertexSize),
//            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//            VK_SHARING_MODE_EXCLUSIVE))
//        , m_vertexCount(static_cast<uint32_t>(vertexCount))
//    {}
//
//    uint32_t vertexCount() const { return m_vertexCount; }
//
//private:
//    uint32_t m_vertexCount;
//};
//
//template <typename VertexData>
//class VertexBufferT : public VertexBuffer
//{
//public:
//    VertexBufferT(const Device& device, const std::span<const VertexData> vertices)
//        : VertexBuffer(device, vertices.size(), sizeof(VertexData))
//    {}
//};
//
//class IndexBuffer: public Buffer
//{
//public:
//    IndexBuffer(const Device& device, size_t indexCount, size_t indexSize)
//        : Buffer(device, create::bufferCreateInfo(
//            static_cast<VkDeviceSize>(indexCount * indexSize),
//            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//            VK_SHARING_MODE_EXCLUSIVE))
//        , m_indexCount(static_cast<uint32_t>(indexCount))
//    {}
//
//    uint32_t indexCount() const { return m_indexCount; }
//
//private:
//    uint32_t m_indexCount;
//};
//
//template <typename IndexData>
//class IndexBufferT : public IndexBuffer
//{
//public:
//    IndexBufferT(const Device& device, const std::span<const IndexData> indices)
//        : IndexBuffer(device, indices.size(), sizeof(IndexData))
//    {}
//};

}
