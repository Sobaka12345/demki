#pragma once

#include "memory.hpp"

namespace vk { namespace handles {

template <typename Impl>
class SIMemoryAccessor
{
protected:
    static uint32_t findMemoryType(
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
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

public:
    SIMemoryAccessor(SIMemoryAccessor&& other) noexcept
        : m_device(std::move(other.m_device))
        , m_memory(std::move(other.m_memory))
    {}

    SIMemoryAccessor(const SIMemoryAccessor& other) = delete;

    SIMemoryAccessor(const Device& device) noexcept
        : m_device(device)
    {}

    virtual ~SIMemoryAccessor(){};

    const Device& device() const { return m_device; }

    std::weak_ptr<Memory> memory() const { return m_memory; }

    bool bindMemory(uint32_t bindingOffset) { return impl()->bindMemory(bindingOffset); }

    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
    {
        return impl()->allocateMemory(properties);
    }

    std::weak_ptr<Memory> allocateAndBindMemory(VkMemoryPropertyFlags properties,
        uint32_t bindingOffset = 0)
    {
        allocateMemory(properties);
        ASSERT(bindMemory(bindingOffset));
        return m_memory;
    }

private:
    Impl* impl() { return static_cast<Impl*>(this); }

protected:
    const Device& m_device;
    //  TO DO: Implement multiple memory allocations
    std::shared_ptr<Memory> m_memory;
};

}}    //  namespace vk::handles
