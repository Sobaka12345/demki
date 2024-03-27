#include "opengl_window.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <assert.hpp>

namespace shell::glfw {

OpenGLWindow::OpenGLWindow(int width, int height, std::string name)
    : Window(GLFW_OPENGL_API, width, height, name)
{}

OpenGLWindow::~OpenGLWindow() {}

void OpenGLWindow::swapBuffers()
{
    glfwSwapBuffers(glfwHandle());
}

renderer::IGraphicsContext& OpenGLWindow::graphicsContext()
{
    if (m_graphicsContext)
    {
        return *m_graphicsContext;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwMakeContextCurrent(create());

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to initialize opengl");
    auto newContext = createContext();

    m_graphicsContext.reset(newContext);
    m_swapchain = newContext->createSwapchain(*this, {});

    return *m_graphicsContext;
}

bool OpenGLWindow::prepare(renderer::OperationContext& context)
{
    glfwMakeContextCurrent(glfwHandle());
    return m_swapchain->prepare(context);
}

void OpenGLWindow::present(renderer::OperationContext& context)
{
    return m_swapchain->present(context);
}

void OpenGLWindow::accept(renderer::RenderInfoVisitor& visitor) const
{
    m_swapchain->accept(visitor);
}


}    //  namespace shell::glfw
