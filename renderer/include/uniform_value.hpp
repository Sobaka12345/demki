#pragma once

#include "iuniform_buffer.hpp"

#include <memory>

namespace renderer {

template <typename T>
struct UniformValue
{
    UniformValue(std::shared_ptr<IUniformBuffer> source)
        : source(source)
    {}

    operator std::shared_ptr<IShaderResource>() { return source; }

    void set(const T& value) { source->write(&value, sizeof(T)); }

    T& get() { return *static_cast<T*>(source->read(sizeof(T))); }

    const T& get() const { return *static_cast<const T*>(source->read(sizeof(T))); }

    std::shared_ptr<IUniformBuffer> source;
};

}    //  namespace renderer
