#include "vulkan_window.hpp"

#include <GLFW/glfw3.h>

namespace shell::glfw {

VulkanWindow::VulkanWindow(int width, int height, std::string name)
    : Window(GLFW_NO_API, width, height, name)
{}

VulkanWindow::~VulkanWindow()
{
    m_swapchain.reset();
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    m_graphicsContext.reset();
}

VkSurfaceKHR VulkanWindow::surfaceKHR() const
{
    return m_surface;
}

renderer::IGraphicsContext& VulkanWindow::graphicsContext()
{
    if (m_graphicsContext)
    {
        return *m_graphicsContext;
    }

    auto newContext = createContext(
        renderer::vk::handles::ApplicationInfo()
            .pApplicationName(name().c_str())
            .applicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .pEngineName("DemkiEngine")
            .engineVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
            .apiVersion(VK_API_VERSION_1_3));

    m_instance = *newContext;
    glfwCreateWindowSurface(m_instance, create(), nullptr, &m_surface);

    m_graphicsContext.reset(newContext);
    newContext->init(*this);
    m_swapchain = newContext->createSwapchain(*this, {});

    return *m_graphicsContext;
}

bool VulkanWindow::prepare(renderer::OperationContext& context)
{
    return m_swapchain->prepare(context);
}

void VulkanWindow::present(renderer::OperationContext& context)
{
    m_swapchain->present(context);
}

void VulkanWindow::accept(renderer::RenderInfoVisitor& visitor) const
{
    m_swapchain->accept(visitor);
}

}    //  namespace shell::glfw
