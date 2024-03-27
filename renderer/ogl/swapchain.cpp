#include "swapchain.hpp"

#include "graphics_context.hpp"

#include <iopengl_surface.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

Swapchain::Swapchain(GraphicsContext& context, IOpenGLSurface& surface, CreateInfo createInfo)
    : m_context(context)
    , m_surface(surface)
    , m_framebufferSize(surface.framebufferSize())
{
    surface.registerFramebufferResizeCallback([this](int width, int height) {
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

bool Swapchain::prepare(renderer::OperationContext& context)
{
    get(context).specificTarget = this;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void Swapchain::present(renderer::OperationContext& context)
{
    m_surface.swapBuffers();
}

uint32_t Swapchain::framesInFlight() const
{
    return 1;
}

GLuint Swapchain::framebuffer()
{
    return 0;
}

}    //  namespace renderer::ogl
