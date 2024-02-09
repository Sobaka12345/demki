#include "vulkan_window.hpp"

#include <GLFW/glfw3.h>

namespace shell::glfw {

VulkanWindow::VulkanWindow(int width, int height, std::string name)
    : Window(GLFW_NO_API, width, height, name)
{}

VulkanWindow::~VulkanWindow() {}

void VulkanWindow::init(VkInstance instance)
{
    m_instance = instance;
    glfwCreateWindowSurface(instance, glfwWindow(), nullptr, &m_surface);
}

void VulkanWindow::destroy()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

VkSurfaceKHR VulkanWindow::surfaceKHR() const
{
    return m_surface;
}


}    //  namespace shell::glfw
