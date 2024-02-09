#pragma once

#include "window.hpp"

#include <ivulkan_window.hpp>

#include <string>
#include <vector>
#include <functional>

class GLFWwindow;

namespace shell::glfw {

class VulkanWindow
    : public IVulkanWindow
    , public Window
{
public:
    VulkanWindow(int width, int height, std::string name);
    ~VulkanWindow();

    virtual VkSurfaceKHR surfaceKHR() const override;

private:
    virtual void init(VkInstance instance) override;
    virtual void destroy() override;

private:
    VkSurfaceKHR m_surface;
    VkInstance m_instance;
};

}    //  namespace shell::glfw
