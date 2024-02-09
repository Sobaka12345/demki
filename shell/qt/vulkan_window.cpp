#include "vulkan_window.hpp"

#include <assert.hpp>

#include <QOpenGLWindow>
#include <QVulkanWindow>

namespace shell::qt {

VulkanWindow::VulkanWindow(int width, int height, std::string name, QWindow* parent)
{
    m_window = new QWindow(parent);
    m_window->setSurfaceType(QWindow::VulkanSurface);
    m_window->setWidth(width);
    m_window->setHeight(height);
    m_window->setTitle(QString::fromStdString(name));
}

void VulkanWindow::init(VkInstance instance)
{
    m_instance.setVkInstance(instance);
    ASSERT(m_instance.create(), "failed to create qt vulkan instance");

    m_window->create();
    m_window->show();
    m_window->setVulkanInstance(&m_instance);

    m_surface = m_instance.surfaceForWindow(m_window);
}

void VulkanWindow::destroy()
{
    m_window->destroy();
    m_instance.destroy();
}

VulkanWindow::~VulkanWindow()
{
    if (!m_window->parent())
    {
        delete m_window;
    }
}

QWindow* VulkanWindow::qWindow()
{
    return m_window;
}

const QWindow* VulkanWindow::qWindow() const
{
    return m_window;
}

VkSurfaceKHR VulkanWindow::surfaceKHR() const
{
    return m_surface;
}

}    //  namespace shell::qt
