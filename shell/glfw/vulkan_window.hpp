#pragma once

#include "window.hpp"

#include <ivulkan_window.hpp>

#include <string>

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
    virtual void waitEvents() const override;
    virtual renderer::IGraphicsContext& graphicsContext() override;

    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual void accept(renderer::RenderInfoVisitor& visitor) const override;

private:
    virtual std::vector<const char*> validationLayers() override;
    virtual std::vector<const char*> requiredExtensions() override;

private:
    VkSurfaceKHR m_surface;
    VkInstance m_instance;

    std::shared_ptr<renderer::ISwapchain> m_swapchain;
    std::shared_ptr<renderer::IGraphicsContext> m_graphicsContext;
};

}    //  namespace shell::glfw
