#include "opengl_window.hpp"

#include <assert.hpp>
#include <glad/glad.h>

#include <QOpenGLWindow>
#include <QOpenGLContext>
#include <QSurfaceFormat>

namespace shell::qt {

OpenGLWindow::OpenGLWindow(int width, int height, std::string name, QWindow* parent)
    : Window(width, height, name, parent)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
    m_swapchain.reset();
    m_graphicsContext.reset();
}

void OpenGLWindow::swapBuffers()
{
    m_context->swapBuffers(this);
}

renderer::IGraphicsContext& OpenGLWindow::graphicsContext()
{
    if (m_graphicsContext)
    {
        return *m_graphicsContext;
    }

    create();
    show();

    m_context = new QOpenGLContext(this);

    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(6);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setOption(QSurfaceFormat::FormatOption::DebugContext);
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
    m_context->setFormat(format);
    m_context->create();

    ASSERT(m_context->makeCurrent(this), "can't make context current");
    ASSERT(gladLoadGL(), "failed to initialize opengl");

    auto newContext = createContext();

    m_graphicsContext.reset(newContext);
    m_swapchain = newContext->createSwapchain(*this, {});

    return *m_graphicsContext;
}

bool OpenGLWindow::prepare(renderer::OperationContext& context)
{
    m_context->makeCurrent(this);
    return m_swapchain->prepare(context);
}

void OpenGLWindow::present(renderer::OperationContext& context)
{
    m_swapchain->present(context);
}

void OpenGLWindow::accept(renderer::RenderInfoVisitor& visitor) const
{
    m_swapchain->accept(visitor);
}


}    //  namespace shell::qt
