#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>
#include <list>
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
            void write(const void* src, VkDeviceSize size);

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
    static Buffer indexBuffer(const Device& device, VkDeviceSize size);
    static Buffer vertexBuffer(const Device& device, VkDeviceSize size);
    static Buffer stagingBuffer(const Device& device, VkDeviceSize size);

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

}
