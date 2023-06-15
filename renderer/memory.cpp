#include "memory.hpp"

namespace vk {

Memory::Mapped::Mapped(const Memory& memory, VkMemoryMapFlags flags, VkDeviceSize offset)
    : memory(memory)
    , offset(offset)
{
    ASSERT(vkMapMemory(memory.device, memory,
        offset, memory.size, flags, &data) == VK_SUCCESS);
}

Memory::Mapped::~Mapped()
{
    vkUnmapMemory(memory.device, memory);
}

void Memory::Mapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    std::memcpy(
        reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + offset),
        src, static_cast<size_t>(size));
}

void Memory::Mapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    const auto memoryRange = create::mappedMemoryRange(memory, offset, size);
    vkFlushMappedMemoryRanges(memory.device, 1, &memoryRange);
}

void Memory::Mapped::writeAndSync(const void *src, VkDeviceSize size, ptrdiff_t offset)
{
    write(src, size, offset);
    sync(size, offset);
}

Memory::Memory(const Device& device, VkMemoryAllocateInfo allocInfo)
    : device(device)
    , size(allocInfo.allocationSize)
    , memoryType(allocInfo.memoryTypeIndex)
{
    ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &m_handle) == VK_SUCCESS);
}

Memory::Memory(Memory&& other)
    : HandleBase(std::move(other))
    , device(other.device)
    , size(std::move(other.size))
    , mapped(std::move(other.mapped))
    , memoryType(std::move(other.memoryType))
{}

Memory::~Memory()
{
    mapped.reset();
    vkFreeMemory(device, m_handle, nullptr);
}

std::weak_ptr<Memory::Mapped> Memory::map(VkMemoryMapFlags flags, VkDeviceSize offset)
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
