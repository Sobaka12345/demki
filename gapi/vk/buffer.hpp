#ifndef GAPI_VK_BUFFER_HPP
#define GAPI_VK_BUFFER_HPP

#include "../gapi_fwd.hpp"
#include <vulkan/vulkan.h>

namespace gapi::__private {

template<>
struct Buffer<Vk> {
    VkBuffer handle;

    void write(const void *data, uint64_t size, uint64_t offset) noexcept;
    [[nodiscard]] const void* read(uint64_t size, uint64_t offset) const noexcept;
    [[nodiscard]] uint64_t size() const noexcept;
};

} // namespace gapi

#endif // GAPI_VK_BUFFER_HPP
