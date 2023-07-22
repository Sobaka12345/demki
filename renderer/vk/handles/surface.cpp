#include "surface.hpp"

#include "instance.hpp"

#include <GLFW/glfw3.h>

namespace vk { namespace handles {

Surface::Surface(Surface&& other)
    : Handle(std::move(other))
    , m_instance(other.m_instance)
{}

Surface::Surface(const Instance& instance, GLFWwindow* window, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_instance(instance)
{
    ASSERT(create(glfwCreateWindowSurface, instance, window, nullptr) == VK_SUCCESS,
        "failed to create window surface");
}

Surface::~Surface()
{
    destroy(vkDestroySurfaceKHR, m_instance, handle(), nullptr);
}

}}    //  namespace vk::handles
