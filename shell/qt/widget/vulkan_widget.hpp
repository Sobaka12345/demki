#pragma once

#include "widget.hpp"
#include "../window/vulkan_window.hpp"

#include <ivulkan_surface.hpp>

namespace shell::qt {

class VulkanWidget
    : public Widget
    , public renderer::IVulkanSurface
{
public:
    VulkanWidget(QWidget* parent = nullptr);

    virtual VkSurfaceKHR surfaceKHR() const override;

private:
    virtual Window* window() override;
    virtual const Window* window() const override;

private:
    VulkanWindow* m_window;
};

}    //  namespace shell::qt
