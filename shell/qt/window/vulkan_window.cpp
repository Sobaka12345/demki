#include "vulkan_window.hpp"

#include <assert.hpp>

#include <QCoreApplication>
#include <QOpenGLWindow>
#include <QVulkanWindow>

namespace shell::qt {

VulkanWindow::VulkanWindow(int width, int height, std::string name, QWindow* parent)
    : Window(width, height, name, parent)
{
    setSurfaceType(QWindow::VulkanSurface);

    connect(this, &Window::aboutToClose, this, [this]() { m_swapchain.reset(); });
}

VulkanWindow::~VulkanWindow()
{
    m_instance.destroy();
    m_graphicsContext.reset();
}

VkSurfaceKHR VulkanWindow::surfaceKHR() const
{
    return m_surface;
}

void VulkanWindow::waitEvents() const
{
    QCoreApplication::processEvents(QEventLoop::AllEvents);
}

bool VulkanWindow::prepare(renderer::OperationContext& context)
{
    return m_swapchain->prepare(context);
}

void VulkanWindow::present(renderer::OperationContext& context)
{
    m_swapchain->present(context);
}

std::vector<const char*> VulkanWindow::validationLayers()
{
    // TO DO
    return {};
}

std::vector<const char*> VulkanWindow::requiredExtensions()
{
    std::vector<const char*> result;
    m_instance.create();
    for (auto extension : m_instance.extensions()) result.push_back(extension.constData());
    m_instance.destroy();
#ifndef NDEBUG
    result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return result;
}

renderer::IGraphicsContext& VulkanWindow::graphicsContext()
{
    if (m_graphicsContext)
    {
        return *m_graphicsContext;
    }

    auto newContext = createContext();

    m_instance.setVkInstance(*newContext);
    ASSERT(m_instance.create(), "failed to create qt vulkan instance");

    create();
    show();
    setVulkanInstance(&m_instance);

    m_surface = m_instance.surfaceForWindow(this);

    m_graphicsContext.reset(newContext);
    newContext->init(*this);

    m_swapchain = newContext->createSwapchain(*this, {});

    return *m_graphicsContext;
}

void VulkanWindow::accept(renderer::RenderInfoVisitor& visitor) const
{
    return m_swapchain->accept(visitor);
}

}    //  namespace shell::qt
