#include "surface.hpp"

#include "instance.hpp"

namespace renderer::vk { namespace handles {

Surface::Surface(Surface&& other) noexcept
    : Handle(std::move(other))
    , m_instance(other.m_instance)
{}

// Surface::Surface(const Instance& instance, GLFWwindow* window, VkHandleType* handlePtr) noexcept
//     : Handle(handlePtr)
//     , m_instance(instance)
// {
//     ASSERT(create(glfwCreateWindowSurface, instance, window, nullptr) == VK_SUCCESS,
//         "failed to create window surface");
// }

// Surface::Surface(const Instance& instance, GLFWwindow* window) noexcept
//     : Surface(instance, window, nullptr)
// {}

// Surface::~Surface()
// {
//     destroy(vkDestroySurfaceKHR, m_instance, handle(), nullptr);
// }

}}    //  namespace renderer::vk::handles
