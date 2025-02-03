#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

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

template <typename T>
struct MemoryFunctions : public CRTPBase<T>
{
    static inline uint32_t findMemoryType(
        VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        ASSERT(false, "failed to find suitable memory type!");
        return 666;
    }

    //  void Memory::HostVisibleMapped::sync(VkDeviceSize size, ptrdiff_t offset)
    //  {
    //      const auto atomSize =
    //      memory.device.physicalDeviceProperties().limits.nonCoherentAtomSize;

    //    if (auto rem = offset % atomSize; rem != 0) offset = offset - rem;

    //    //  TO DO: reorganize memory flushing
    //    if (size < atomSize) size = atomSize;
    //    else if (auto rem = size % atomSize; size > atomSize && size < memory.size && rem != 0)
    //    {
    //        size = size - rem + atomSize;
    //    }

    //    if (size + offset > memory.size) size = memory.size - offset;

    //    const auto range = MappedMemoryRange{}.memory(memory).offset(offset).size(size);

    //    vkFlushMappedMemoryRanges(memory.device, 1, &range);
    //  }
};

template <typename T>
struct MemoryGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE_FULL(
    DeviceMemory, vkAllocateMemory, vkFreeMemory, MemoryFunctions, MemoryGroupFunctions);
}

}    //  namespace renderer::vk
