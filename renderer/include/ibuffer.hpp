#pragma once

#include <cstddef>

namespace renderer {

class IBuffer
{
public:
    template <typename CreateInfo>
    struct CreateInfoBase
    {
        CREATE_INFO_PROPERTY(size_t, size, 0)
    };

public:
    virtual void write(const void* data, size_t size, size_t offset = 0) = 0;
    virtual const void* read(size_t size, size_t offset = 0) const = 0;

    virtual size_t size() const = 0;
    virtual void reallocate(size_t newSize) = 0;

public:
    virtual ~IBuffer() {};
};

}    //  namespace renderer
