#ifndef GAPI_BUFFER_HPP
#define GAPI_BUFFER_HPP

#include "gapi.hpp"
#include "../vk/buffer.hpp"

namespace gapi 
{

template <typename T>
concept SIBuffer = requires(
    T& buffer,
    const T& constBuffer,
    const void* data,
    std::size_t size,
    std::size_t offset) {
    { buffer.handle };
    { buffer.write(data, size, offset) } -> std::same_as<void>;
    { constBuffer.read(size, offset) } -> std::convertible_to<const void*>;
    { constBuffer.size() } -> std::same_as<uint64_t>;
};

template <typename GApiT>
    requires SIBuffer<__private::Buffer<GApiT>>
struct Buffer : __private::Buffer<GApiT>
{};

}


#endif // GAPI_BUFFER_HPP