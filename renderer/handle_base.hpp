#pragma once

#include "assert.hpp"

#include <vulkan/vulkan_core.h>

namespace vk {

template <typename HandlerType>
class HandleBase
{
public:
    HandleBase()
    {
        m_handle = VK_NULL_HANDLE;
    }

    virtual ~HandleBase()
    {
        m_handle = VK_NULL_HANDLE;
    }

    HandleBase(HandleBase&& other)
        : m_handle(other.m_handle)
    {
        other.m_handle = VK_NULL_HANDLE;
    }

    HandlerType handle() const { return m_handle; }
    operator HandlerType() const { return m_handle; }

    HandleBase(const HandleBase& other) = delete;
    HandleBase& operator=(const HandleBase& other) = delete;

protected:
    HandlerType m_handle;
};

}