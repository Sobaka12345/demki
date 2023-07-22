#pragma once

#include "handle.hpp"

class GLFWwindow;

namespace vk { namespace handles {

class Instance;

class Surface : public Handle<VkSurfaceKHR>
{
public:
    Surface(const Surface& other) = delete;
    Surface(Surface&& other);
    Surface(const Instance& instance, GLFWwindow* window, VkHandleType* handlePtr = nullptr);
    ~Surface();

private:
    const Instance& m_instance;
};

}}    //  namespace vk::handles
