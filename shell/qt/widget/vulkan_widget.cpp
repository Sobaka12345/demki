#include "vulkan_widget.hpp"

#include "../window/vulkan_window.hpp"

namespace shell::qt {

VulkanWidget::VulkanWidget(QWidget* parent)
    : Widget(parent)
{
    m_window = new VulkanWindow(QWidget::width(), QWidget::height(), "");
    m_windowContainer = QWidget::createWindowContainer(m_window, this);
}

VkSurfaceKHR VulkanWidget::surfaceKHR() const
{
    return m_window->surfaceKHR();
}

Window* VulkanWidget::window()
{
    return m_window;
}

const Window* VulkanWidget::window() const
{
    return m_window;
}

}    //  namespace shell::qt
