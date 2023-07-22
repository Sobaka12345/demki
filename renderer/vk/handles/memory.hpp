#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>
#include <list>
#include <span>
#include <memory>
#include <optional>

namespace vk { namespace handles {

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

    Memory(const Device& device, VkMemoryAllocateInfo size, VkHandleType* handlePtr = nullptr);
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
