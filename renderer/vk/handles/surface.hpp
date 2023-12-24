#pragma once

#include "handle.hpp"

class GLFWwindow;

namespace vk { namespace handles {

class Instance;

class Surface : public Handle<VkSurfaceKHR>
{
    HANDLE(Surface);

public:
    Surface(const Surface& other) = delete;
    Surface(Surface&& other) noexcept;
    Surface(const Instance& instance, GLFWwindow* window) noexcept;
    virtual ~Surface();

protected:
    Surface(const Instance& instance, GLFWwindow* window, VkHandleType* handlePtr) noexcept;

private:
    const Instance& m_instance;
};

}}    //  namespace vk::handles
