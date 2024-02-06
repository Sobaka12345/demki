#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include "buffer.hpp"
#include "image.hpp"

#include <memory>
#include <variant>

namespace vk { namespace handles {

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

class Buffer;
class Image;
class Device;

struct Memory : public Handle<VkDeviceMemory>
{
    HANDLE(Memory);

public:
    struct Mapped
    {
        Mapped(const Memory& memory);
        virtual ~Mapped();
        virtual const void* read(VkDeviceSize size, ptrdiff_t offset = 0) const = 0;
        virtual void write(const void* src, VkDeviceSize size, ptrdiff_t offset = 0) = 0;
        virtual void sync(VkDeviceSize size, ptrdiff_t offset = 0) = 0;
        void writeAndSync(const void* src, VkDeviceSize size, ptrdiff_t offset = 0);

        const Memory& memory;
    };

    struct DeviceLocalMapped : public Mapped
    {
        DeviceLocalMapped(const Memory& memory, VkDeviceSize offset = 0);
        ~DeviceLocalMapped();
        virtual const void* read(VkDeviceSize size, ptrdiff_t offset = 0) const override;
        virtual void write(const void* src, VkDeviceSize size, ptrdiff_t offset = 0) override;
        virtual void sync(VkDeviceSize size, ptrdiff_t offset = 0) override;

        std::unique_ptr<Buffer> stagingBuffer;
        VkDeviceSize offset;
    };

    struct HostVisibleMapped : public Mapped
    {
        HostVisibleMapped(
            const Memory& memory, VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
        ~HostVisibleMapped();
        virtual const void* read(VkDeviceSize size, ptrdiff_t offset = 0) const override;
        virtual void write(const void* src, VkDeviceSize size, ptrdiff_t offset = 0) override;
        virtual void sync(VkDeviceSize size, ptrdiff_t offset = 0) override;

        void* data;
    };

public:
    Memory(const Device& buffer, MemoryAllocateInfo allocInfo) noexcept;
    Memory(Memory&& other) noexcept;
    virtual ~Memory();

    bool bindImage(const Image& image, uint32_t offset = 0);
    bool bindBuffer(const Buffer& buffer, uint32_t offset = 0);

    const Buffer& buffer() const;
    const Image& image() const;

    std::weak_ptr<Mapped> map(VkMemoryMapFlags flags = 0, VkDeviceSize offset = 0);
    void unmap();

    const Device& device;
    VkDeviceSize size;
    std::shared_ptr<Mapped> mapped;
    VkMemoryType memoryType;

protected:
    Memory(const Device& device, MemoryAllocateInfo allocInfo, VkHandleType* handlePtr) noexcept;

private:
    std::variant<const Buffer*, const Image*> bindedResource;
};

}}    //  namespace vk::handles
