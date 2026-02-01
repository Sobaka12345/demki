#include "buffer.hpp"

namespace gapi::__private {
    void Buffer<Vk>::write(const void *data, uint64_t size, uint64_t offset) noexcept
    {
        (void) data;
        (void) size;
        (void) offset;
    }

    const void* Buffer<Vk>::read(uint64_t size, uint64_t offset) const noexcept
    {
        (void) size;
        (void) offset;
        return nullptr;
    }

    uint64_t Buffer<Vk>::size() const noexcept
    {
        return 0;
    }
}