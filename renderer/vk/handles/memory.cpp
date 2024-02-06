#include "memory.hpp"

#include "buffer.hpp"
#include "device.hpp"

#include <cstring>

namespace vk { namespace handles {

Memory::Mapped::Mapped(const Memory& memory)
    : memory(memory)
{}

Memory::Mapped::~Mapped() {}

void Memory::Mapped::writeAndSync(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    write(src, size, offset);
    sync(size, offset);
}

Memory::DeviceLocalMapped::DeviceLocalMapped(const Memory& memory, VkDeviceSize offset)
    : Mapped(memory)
    , offset(offset)
{
    stagingBuffer = std::make_unique<Buffer>(memory.device, Buffer::staging().size(memory.size));
    stagingBuffer
        ->allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        .lock()
        ->map();
}

Memory::DeviceLocalMapped::~DeviceLocalMapped() {}

const void* Memory::DeviceLocalMapped::read(VkDeviceSize size, ptrdiff_t offset) const
{
    ASSERT(false, "not implemented");
    return nullptr;
}

void Memory::DeviceLocalMapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    stagingBuffer->memory().lock()->mapped->writeAndSync(src, size, offset);
}

//  why ptrdiff?
void Memory::DeviceLocalMapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    stagingBuffer->copyTo(memory.buffer(),
        {
            .srcOffset = static_cast<VkDeviceSize>(offset),
            .dstOffset = this->offset + static_cast<VkDeviceSize>(offset),
            .size = size,
        });
}

Memory::HostVisibleMapped::HostVisibleMapped(
    const Memory& memory, VkMemoryMapFlags flags, VkDeviceSize offset)
    : Mapped(memory)
{
    ASSERT(vkMapMemory(memory.device, memory, offset, memory.size, flags, &data) == VK_SUCCESS);
}

Memory::HostVisibleMapped::~HostVisibleMapped()
{
    vkUnmapMemory(memory.device, memory);
}

const void* Memory::HostVisibleMapped::read(VkDeviceSize size, ptrdiff_t offset) const
{
    ASSERT(size + offset <= memory.size, "invalid memory range");

    return (static_cast<char*>(data)) + offset;
}

void Memory::HostVisibleMapped::write(const void* src, VkDeviceSize size, ptrdiff_t offset)
{
    std::memcpy(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + offset), src,
        static_cast<size_t>(size));
}

void Memory::HostVisibleMapped::sync(VkDeviceSize size, ptrdiff_t offset)
{
    const auto atomSize = memory.device.physicalDeviceProperties().limits.nonCoherentAtomSize;

    if (auto rem = offset % atomSize; rem != 0) offset = offset - rem;

    //  TO DO: reorganize memory flushing
    if (size < atomSize) size = atomSize;
    else if (auto rem = size % atomSize; size > atomSize && size < memory.size && rem != 0)
    {
        size = size - rem + atomSize;
    }

    if (size + offset > memory.size) size = memory.size - offset;

    const auto range = MappedMemoryRange{}.memory(memory).offset(offset).size(size);

    vkFlushMappedMemoryRanges(memory.device, 1, &range);
}

Memory::Memory(Memory&& other) noexcept
    : Handle(std::move(other))
    , device(other.device)
    , bindedResource(std::move(other.bindedResource))
    , size(std::move(other.size))
    , mapped(std::move(other.mapped))
    , memoryType(std::move(other.memoryType))
{}

Memory::Memory(const Device& device, MemoryAllocateInfo allocInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , device(device)
    , size(allocInfo.allocationSize())
    , memoryType(device.memoryType(allocInfo.memoryTypeIndex()))
{
    ASSERT(create(vkAllocateMemory, device, &allocInfo, nullptr) == VK_SUCCESS);
}

Memory::Memory(const Device& device, MemoryAllocateInfo allocInfo) noexcept
    : Memory(device, std::move(allocInfo), nullptr)
{}

Memory::~Memory()
{
    mapped.reset();
    destroy(vkFreeMemory, device, handle(), nullptr);
}

bool Memory::bindImage(const Image& image, uint32_t offset)
{
    bindedResource = &image;
    return vkBindImageMemory(device, image, *this, offset) == VK_SUCCESS;
}

bool Memory::bindBuffer(const Buffer& buffer, uint32_t offset)
{
    bindedResource = &buffer;
    return vkBindBufferMemory(device, buffer, *this, offset) == VK_SUCCESS;
}

const Buffer& Memory::buffer() const
{
    DASSERT(std::holds_alternative<const Buffer*>(bindedResource) &&
            std::get<const Buffer*>(bindedResource) != nullptr,
        "invalid buffer value");
    return *std::get<const Buffer*>(bindedResource);
}

const Image& Memory::image() const
{
    DASSERT(std::holds_alternative<const Image*>(bindedResource) &&
            std::get<const Image*>(bindedResource) != nullptr,
        "invalid image value");
    return *std::get<const Image*>(bindedResource);
}

std::weak_ptr<Memory::Mapped> Memory::map(VkMemoryMapFlags flags, VkDeviceSize offset)
{
    DASSERT(!mapped);
    if (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        mapped = std::make_shared<HostVisibleMapped>(*this, flags, offset);
    }
    else
    {
        mapped = std::make_shared<DeviceLocalMapped>(*this, offset);
    }

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
