#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>
#include <list>
#include <span>
#include <memory>
#include <optional>

namespace vk {

class Buffer
{
public:
    struct Memory
    {
        struct Mapped
        {
            Mapped(const Memory& memory, VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
            ~Mapped();
            void write(const void* src, VkDeviceSize size, ptrdiff_t offset = 0);
            void sync(VkDeviceSize size, ptrdiff_t offset = 0);
            void writeAndSync(const void* src, VkDeviceSize size, ptrdiff_t offset = 0);

            const Memory& memory;
            void* data;
            VkDeviceSize offset;
        };

        Memory(const Buffer& buffer, VkMemoryAllocateInfo size);
        ~Memory();

        std::shared_ptr<Mapped> map(VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
        void unmap();

        const Buffer& buffer;
        VkDeviceSize size;
        VkDeviceMemory deviceMemory;
        std::shared_ptr<Mapped> mapped;
        uint32_t memoryType;
    };

public:
    Buffer(Buffer&& other);
    Buffer(const Buffer& other) = delete;
    Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage,
        VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);
    ~Buffer();

    VkBuffer buffer() const { return m_buffer; }
    VkDevice device() const { return m_device; }
    VkDeviceSize size() const { return m_size; }
    std::shared_ptr<Memory> memory() const { return m_memory; }

    bool bindMemory(uint32_t bindingOffset);
    std::shared_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties);
    std::shared_ptr<Memory> allocateAndBindMemory(VkMemoryPropertyFlags properties, uint32_t bindingOffset = 0);

    void copyTo(const Buffer& buffer, VkCommandPool commandPool, VkQueue queue, VkBufferCopy copyRegion);

protected:
    VkBuffer m_buffer;
    VkDeviceSize m_size;
    const Device& m_device;
    // TO DO: Implement multiple memory allocations
    std::shared_ptr<Memory> m_memory;
};

class StagingBuffer: public Buffer
{
public:
    StagingBuffer(const Device& device, VkDeviceSize size)
        : Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE)
    {}
};

class VertexBuffer: public Buffer
{
public:
    VertexBuffer(const Device& device, size_t vertexCount, size_t vertexSize)
        : Buffer(device, static_cast<VkDeviceSize>(vertexCount * vertexSize),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE)
        , m_vertexCount(static_cast<uint32_t>(vertexCount))
    {}

    uint32_t vertexCount() const { return m_vertexCount; }

private:
    uint32_t m_vertexCount;
};

template <typename VertexData>
class VertexBufferT : public VertexBuffer
{
public:
    VertexBufferT(const Device& device, const std::span<const VertexData> vertices)
        : VertexBuffer(device, vertices.size(), sizeof(VertexData))
    {}
};

class IndexBuffer: public Buffer
{
public:
    IndexBuffer(const Device& device, size_t indexCount, size_t indexSize)
        : Buffer(device, static_cast<VkDeviceSize>(indexCount * indexSize),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE)
        , m_indexCount(static_cast<uint32_t>(indexCount))
    {}

    uint32_t indexCount() const { return m_indexCount; }

private:
    uint32_t m_indexCount;
};

template <typename IndexData>
class IndexBufferT : public IndexBuffer
{
public:
    IndexBufferT(const Device& device, const std::span<const IndexData> indices)
        : IndexBuffer(device, indices.size(), sizeof(IndexData))
    {}
};

}
