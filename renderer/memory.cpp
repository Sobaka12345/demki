#include "memory.hpp"

namespace vk {

Memory::Mapped::Mapped(const Memory& memory, VkMemoryMapFlags flags, VkDeviceSize offset)
    : memory(memory)
    , offset(offset)
{
    ASSERT(vkMapMemory(memory.buffer.device(), memory.deviceMemory,
        offset, memory.size, flags, &data) == VK_SUCCESS);
}

Memory::Mapped::~Mapped()
{
    vkUnmapMemory(memory.buffer.device(), memory.deviceMemory);
}

void Memory::Mapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    std::memcpy(
        reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + offset),
        src, static_cast<size_t>(size));
}

void Memory::Mapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    VkMappedMemoryRange memoryRange{};
    memoryRange.memory = memory.deviceMemory;
    memoryRange.size = size;
    memoryRange.offset = static_cast<VkDeviceSize>(offset);
    vkFlushMappedMemoryRanges(memory.buffer.device(), 1, &memoryRange);
}

void Memory::Mapped::writeAndSync(const void *src, VkDeviceSize size, ptrdiff_t offset)
{
    write(src, size, offset);
    sync(size, offset);
}

Memory::Memory(const Device& buffer, VkMemoryAllocateInfo allocInfo)
    : buffer(buffer)
    , size(allocInfo.allocationSize)
    , memoryType(allocInfo.memoryTypeIndex)
{
    ASSERT(vkAllocateMemory(buffer.device(), &allocInfo, nullptr, &deviceMemory) == VK_SUCCESS);
}

Memory::~Memory()
{
    vkFreeMemory(buffer.device(), deviceMemory, nullptr);
}

std::shared_ptr<Memory::Mapped> Memory::map(VkMemoryMapFlags flags, VkDeviceSize offset)
{
    DASSERT(!mapped);
    mapped = std::make_shared<Mapped>(*this, flags, offset);

    return mapped;
}

void Memory::unmap()
{
    if (mapped)
    {
        mapped.reset();
    }
}

}
