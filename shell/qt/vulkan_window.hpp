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

    virtual VkSurfaceKHR surfaceKHR() const override;

    virtual QWindow* qWindow() override;
    virtual const QWindow* qWindow() const override;

private:
    virtual void init(VkInstance instance) override;
    virtual void destroy() override;

private:
    VkSurfaceKHR m_surface;
    QVulkanInstance m_instance;
    QWindow* m_window;
};

}    //  namespace shell::qt
