#pragma once

#include "window.hpp"

#include <ivulkan_window.hpp>

#include <QWindow>
#include <QVulkanInstance>

namespace shell::qt {
class VulkanWindow
    : public IVulkanWindow
    , public Window
{
public:
    VulkanWindow(int width, int height, std::string name, QWindow* parent = nullptr);
    ~VulkanWindow();

public:
    std::vector<const char*> validationLayers();
    std::vector<const char*> requiredExtensions();

    virtual VkSurfaceKHR surfaceKHR() const override;
    virtual void waitEvents() const override;

    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;
    virtual renderer::IGraphicsContext& graphicsContext() override;

    virtual void accept(renderer::RenderInfoVisitor& visitor) const override;

private:
    VkSurfaceKHR m_surface;
    QVulkanInstance m_instance;

    std::shared_ptr<renderer::ISwapchain> m_swapchain;
    std::shared_ptr<renderer::IGraphicsContext> m_graphicsContext;
};

}    //  namespace shell::qt
