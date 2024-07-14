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

void VulkanWindow::waitEvents() const
{
	return glfwWaitEvents();
}

std::vector<const char*> VulkanWindow::validationLayers()
{
    // TO DO
    return {};
}

std::vector<const char*> VulkanWindow::requiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);    //!!!!!!!

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
}

renderer::IGraphicsContext& VulkanWindow::graphicsContext()
{
    if (m_graphicsContext)
    {
        return *m_graphicsContext;
    }

    auto newContext = createContext();

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
