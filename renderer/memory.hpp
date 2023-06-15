#pragma once

#include "creators.hpp"
#include "device.hpp"

#include <vulkan/vulkan.h>
#include <list>
#include <span>
#include <memory>
#include <optional>

namespace vk {

struct Memory : public HandleBase<VkDeviceMemory>
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

    Memory(const Device& device, VkMemoryAllocateInfo size);
    Memory(Memory&& other);
    ~Memory();

    std::weak_ptr<Mapped> map(VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
    void unmap();

    const Device& device;
    VkDeviceSize size;
    std::shared_ptr<Mapped> mapped;
    uint32_t memoryType;
};

template <typename Impl>
class SIMemoryAccessor
{

public:
    SIMemoryAccessor(SIMemoryAccessor&& other)
        : m_device(std::move(other.m_device))
        , m_memory(std::move(other.m_memory))
    {}

    SIMemoryAccessor(const SIMemoryAccessor& other) = delete;

    SIMemoryAccessor(const Device& device,
        VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE)
        : m_device(device)
    {}

    virtual ~SIMemoryAccessor() {};

    VkDevice device() const { return m_device; }
    std::weak_ptr<Memory> memory() const { return m_memory; }

    bool bindMemory(uint32_t bindingOffset)
    {
        return impl()->bindMemory(bindingOffset);
    }

    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
    {
        return impl()->allocateMemory(properties);
    }

    std::weak_ptr<Memory> allocateAndBindMemory(VkMemoryPropertyFlags properties, uint32_t bindingOffset = 0)
    {
        allocateMemory(properties);
        ASSERT(bindMemory(bindingOffset));
        return m_memory;
    }

private:
    Impl* impl() { return static_cast<Impl*>(this); }

protected:
    const Device& m_device;
    // TO DO: Implement multiple memory allocations
    std::shared_ptr<Memory> m_memory;
};

}
