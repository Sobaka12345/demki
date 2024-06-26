#pragma once

#include <cstdint>

namespace shell {

class IResource
{
public:
    IResource()
        : m_id(createId())
    {}

    uint64_t id() const { return m_id; }

    virtual ~IResource() {}

private:
    static uint64_t createId()
    {
        static uint64_t result = 0;
        return result++;
    }

private:
    uint64_t m_id;
};

}    //  namespace shell
