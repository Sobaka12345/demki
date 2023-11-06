#include "memory.hpp"

#include "device.hpp"

#include <cstring>

namespace vk { namespace handles {

Memory::Mapped::Mapped(const Memory& memory, VkMemoryMapFlags flags, VkDeviceSize offset)
    : memory(memory)
    , offset(offset)
{
    ASSERT(vkMapMemory(memory.device, memory, offset, memory.size, flags, &data) == VK_SUCCESS);
}

Memory::Mapped::~Mapped()
{
    vkUnmapMemory(memory.device, memory);
}

void Memory::Mapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    std::memcpy(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + offset), src,
        static_cast<size_t>(size));
}

void Memory::Mapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    const auto atomSize = memory.device.physicalDeviceProperties().limits.nonCoherentAtomSize;

    //  TO DO: reorganize memory flushing
    if (size < atomSize) size = atomSize;
    else if (auto rem = size % atomSize; size > atomSize && size < memory.size && rem != 0)
    {
        size = size - rem + atomSize;
    }

    if (auto rem = offset % atomSize; rem != 0) offset = offset - rem;

    const auto range = MappedMemoryRange{}.memory(memory).offset(offset).size(size);

    vkFlushMappedMemoryRanges(memory.device, 1, &range);
}

void Memory::Mapped::writeAndSync(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    write(src, size, offset);
    sync(size, offset);
}

Memory::Memory(const Device& device, MemoryAllocateInfo allocInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , device(device)
    , size(allocInfo.allocationSize())
    , memoryType(allocInfo.memoryTypeIndex())
{
    ASSERT(create(vkAllocateMemory, device, &allocInfo, nullptr) == VK_SUCCESS);
}

Memory::Memory(Memory&& other)
    : Handle(std::move(other))
    , device(other.device)
    , size(std::move(other.size))
    , mapped(std::move(other.mapped))
    , memoryType(std::move(other.memoryType))
{}

Memory::~Memory()
{
    mapped.reset();
    destroy(vkFreeMemory, device, handle(), nullptr);
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

}}    //  namespace vk::handles
