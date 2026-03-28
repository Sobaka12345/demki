#include "buffer.hpp"

#include <cstring>
#include <limits>

namespace gapi::__private {

void Buffer<Vk>::write(const void* data, uint64_t writeSize, uint64_t offset) noexcept
{
    DASSERT(data && mappedMemory);
    DASSERT(offset <= byteSize && writeSize <= (byteSize - offset));

    auto* dst = static_cast<std::byte*>(mappedMemory) + offset;
    std::memcpy(dst, data, static_cast<std::size_t>(writeSize));

    if ((memoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
    {
        const VkMappedMemoryRange range {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = memory,
            .offset = offset,
            .size = writeSize,
        };
        vkFlushMappedMemoryRanges(device, 1, &range);
    }
}

const void* Buffer<Vk>::read(uint64_t readSize, uint64_t offset) const noexcept
{
    DASSERT(mappedMemory);
    DASSERT(offset <= byteSize && readSize <= (byteSize - offset));

    if ((memoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
    {
        const VkMappedMemoryRange range {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = memory,
            .offset = offset,
            .size = readSize,
        };
        vkInvalidateMappedMemoryRanges(device, 1, &range);
    }

    return static_cast<const std::byte*>(mappedMemory) + offset;
}

uint64_t Buffer<Vk>::size() const noexcept
{
    return byteSize;
}

} // namespace gapi::__private
