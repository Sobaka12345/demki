#include "opengl_window.hpp"

#include <QOpenGLWindow>

namespace shell::qt {

OpenGLWindow::OpenGLWindow(int width, int height, std::string name, QWindow* parent)
{
    m_window = new QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent);
}

OpenGLWindow::~OpenGLWindow()
{
    if (!m_window->parent())
    {
        delete m_window;
    }
}

QWindow* OpenGLWindow::qWindow()
{
    return m_window;
}

const QWindow* OpenGLWindow::qWindow() const
{
    return m_window;
}

void OpenGLWindow::init() {}

void OpenGLWindow::destroy() {}


}    //  namespace shell::qt
