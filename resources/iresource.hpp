#pragma once

#include <cstdint>

namespace resources {

class IResource
{
public:
    enum Flags : uint64_t
    {
        UNDEFINED = 0,
        TYPE_MESH = 0x01,
        TYPE_IMAGE = 0x02,
        TYPE_UNIFORM_BUFFER = 0x04,
        TYPE_STORAGE_BUFFER = 0x08
    };

    IResource(Flags flags)
        : m_id(createId(flags))
    {}

    uint64_t id() const { return m_id; }

    // Resource's size in bytes
    virtual size_t size() const = 0;

    virtual ~IResource() {}

private:
    static uint64_t createId(Flags flags)
    {
        static uint16_t counter = 0;
        const uint64_t result = counter++;
        return flags | (result << ((sizeof(uint64_t) - sizeof(uint16_t)) * 8));
    }

private:
    uint64_t m_id;
};

}    //  namespace resources
