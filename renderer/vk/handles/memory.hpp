#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <memory>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(MappedMemoryRange, VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDeviceMemory, memory)
    VKSTRUCT_PROPERTY(VkDeviceSize, offset)
    VKSTRUCT_PROPERTY(VkDeviceSize, size)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(MemoryAllocateInfo, VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDeviceSize, allocationSize)
    VKSTRUCT_PROPERTY(uint32_t, memoryTypeIndex)
END_DECLARE_VKSTRUCT()

class Device;

struct Memory : public Handle<VkDeviceMemory>
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

    Memory(const Device& device, MemoryAllocateInfo size, VkHandleType* handlePtr = nullptr);
    Memory(Memory&& other);
    ~Memory();

    std::weak_ptr<Mapped> map(VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
    void unmap();

    const Device& device;
    VkDeviceSize size;
    std::shared_ptr<Mapped> mapped;
    uint32_t memoryType;
};

}}    //  namespace vk::handles
