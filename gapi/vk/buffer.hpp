#ifndef GAPI_VK_BUFFER_HPP
#define GAPI_VK_BUFFER_HPP

#include "../gapi_fwd.hpp"
#include <vulkan/vulkan.h>

namespace gapi::__private {

template<>
struct Buffer<Vk> {
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint64_t byteSize = 0;
    VkMemoryPropertyFlags memoryProperties = 0;
    mutable void* mappedMemory = nullptr;

    void write(const void *data, uint64_t size, uint64_t offset) noexcept;
    [[nodiscard]] const void* read(uint64_t size, uint64_t offset) const noexcept;
    [[nodiscard]] uint64_t size() const noexcept;
};

} // namespace gapi

#endif // GAPI_VK_BUFFER_HPP
