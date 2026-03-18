#include "buffer.hpp"

#include <cstring>
#include <limits>

namespace gapi::__private {

namespace {

uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties) noexcept
{
    VkPhysicalDeviceMemoryProperties memProperties {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1U << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return std::numeric_limits<uint32_t>::max();
}

} // namespace


bool Buffer<Vk>::create(
    VkDevice inDevice,
    VkPhysicalDevice inPhysicalDevice,
    uint64_t inSize,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties) noexcept
{
    destroy();

    if (inDevice == VK_NULL_HANDLE || inPhysicalDevice == VK_NULL_HANDLE || inSize == 0) return false;

    device = inDevice;
    physicalDevice = inPhysicalDevice;
    byteSize = inSize;
    memoryProperties = properties;

    const VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = inSize,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &handle) != VK_SUCCESS)
    {
        destroy();
        return false;
    }

    VkMemoryRequirements requirements {};
    vkGetBufferMemoryRequirements(device, handle, &requirements);

    const uint32_t memoryType =
        findMemoryType(physicalDevice, requirements.memoryTypeBits, memoryProperties);
    if (memoryType == std::numeric_limits<uint32_t>::max())
    {
        destroy();
        return false;
    }

    const VkMemoryAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memoryType,
    };

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        destroy();
        return false;
    }

    if (vkBindBufferMemory(device, handle, memory, 0) != VK_SUCCESS)
    {
        destroy();
        return false;
    }

    if ((memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
    {
        if (vkMapMemory(device, memory, 0, inSize, 0, &mappedMemory) != VK_SUCCESS)
        {
            destroy();
            return false;
        }
    }

    return true;
}

void Buffer<Vk>::destroy() noexcept
{
    if (mappedMemory && device != VK_NULL_HANDLE && memory != VK_NULL_HANDLE)
    {
        vkUnmapMemory(device, memory);
        mappedMemory = nullptr;
    }

    if (memory != VK_NULL_HANDLE && device != VK_NULL_HANDLE)
    {
        vkFreeMemory(device, memory, nullptr);
        memory = VK_NULL_HANDLE;
    }

    if (handle != VK_NULL_HANDLE && device != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(device, handle, nullptr);
        handle = VK_NULL_HANDLE;
    }

    device = VK_NULL_HANDLE;
    physicalDevice = VK_NULL_HANDLE;
    byteSize = 0;
    memoryProperties = 0;
}

void Buffer<Vk>::write(const void* data, uint64_t writeSize, uint64_t offset) noexcept
{
    if (!data || !mappedMemory) return;
    if (offset > byteSize || writeSize > (byteSize - offset)) return;

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
    if (!mappedMemory) return nullptr;
    if (offset > byteSize || readSize > (byteSize - offset)) return nullptr;

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
