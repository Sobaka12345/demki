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
        TYPE_IMAGE = 0x02
    };

    IResource(Flags flags)
        : m_id(createId(flags))
    {}

    uint64_t id() const { return m_id; }

    virtual size_t sizeBytes() const = 0;

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
