#include "vulkan_window.hpp"

#include <assert.hpp>

#include <QOpenGLWindow>
#include <QVulkanWindow>

namespace shell::qt {

VulkanWindow::VulkanWindow(int width, int height, std::string name, QWindow* parent)
    : Window(width, height, name, parent)
{
    setSurfaceType(QWindow::VulkanSurface);

    connect(this, &Window::aboutToClose, this, [this]() { m_swapchain.reset(); });

    auto newContext = createContext(
        renderer::vk::ApplicationInfo()
            .pApplicationName(name.c_str())
            .applicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .pEngineName("DemkiEngine")
            .engineVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .apiVersion(VK_API_VERSION_1_3));

    m_instance.setVkInstance(newContext->instance());
    ASSERT(m_instance.create(), "failed to create qt vulkan instance");

    create();
    show();
    setVulkanInstance(&m_instance);

    m_surface = m_instance.surfaceForWindow(this);

    m_graphicsContext.reset(newContext);
    newContext->init(*this);

    m_swapchain = newContext->createSwapchain(*this, {});
}

VulkanWindow::~VulkanWindow()
{
    m_graphicsContext.reset();
}

VkSurfaceKHR VulkanWindow::surfaceKHR() const
{
    return m_surface;
}

bool VulkanWindow::prepare(renderer::OperationContext& context)
{
    return m_swapchain->prepare(context);
}

void VulkanWindow::present(renderer::OperationContext& context)
{
    m_swapchain->present(context);
}

renderer::IGraphicsContext& VulkanWindow::graphicsContext()
{
    return *m_graphicsContext;
}

}    //  namespace shell::qt
