#pragma once

#include <cstddef>

namespace renderer {

class IBuffer
{
public:
    virtual void write(const void* data, size_t size) = 0;
    virtual const void* read(size_t size) const = 0;

public:
    virtual ~IBuffer() {};
};

}
