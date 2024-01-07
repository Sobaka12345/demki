#include "swapchain.hpp"

#include "graphics_context.hpp"

#include <window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ogl {

Swapchain::Swapchain(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_framebufferSize(context.window().framebufferSize())
{
    m_context.window().registerFramebufferResizeCallback([this](int width, int height) {
        m_framebufferSize = { width, height };
    });
}

Swapchain::~Swapchain() {}

uint32_t Swapchain::width() const
{
    return m_framebufferSize.first;
}

uint32_t Swapchain::height() const
{
    return m_framebufferSize.second;
}

void Swapchain::accept(RenderInfoVisitor& visitor) const
{
    visitor.populateRenderInfo(*this);
}

bool Swapchain::prepare(::OperationContext& context)
{
    get(context).specificTarget = this;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void Swapchain::present(::OperationContext& context)
{
    glfwSwapBuffers(m_context.window().glfwHandle());
}

uint32_t Swapchain::framesInFlight() const
{
    return 1;
}

GLuint Swapchain::framebuffer()
{
    return 0;
}

}    //  namespace ogl
