#pragma once

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
        Memory(const Buffer& buffer, VkMemoryAllocateInfo size);
        Memory(const Memory& other) = delete;
        ~Memory();

        const Buffer& buffer;
        VkDeviceSize size;
        VkDeviceMemory deviceMemory;
        uint32_t memoryType;
    };

    struct MappedMemory
    {
        MappedMemory(const Memory& memory, VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
        ~MappedMemory();
        void write(const void* src, VkDeviceSize size);

        void* data;
        VkDeviceSize offset;
        const Memory& memory;
    };

public:
    Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode);
    Buffer(const Buffer& other) = delete;
    ~Buffer();

    VkBuffer buffer() const { return m_buffer; }
    VkDevice device() const { return m_device; }
    VkDeviceSize size() const { return m_size; }


    bool bindMemory(uint32_t bindingOffset);
    const Memory& allocateMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties);
    const Memory& allocateAndBindMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, uint32_t bindingOffset = 0);

    MappedMemory mappedMemory();
    MappedMemory mapMemory(VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
    void unmapMemory();

    void copyTo(const Buffer& buffer, VkCommandPool commandPool, VkQueue queue, VkBufferCopy copyRegion);

protected:
    VkBuffer m_buffer;
    VkDevice m_device;
    VkDeviceSize m_size;
    std::unique_ptr<Memory> m_memory;
    std::unique_ptr<MappedMemory> m_mapped;
};

}
