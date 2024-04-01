#include "opengl_widget.hpp"

#include "../window/opengl_window.hpp"

namespace shell::qt {

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : Widget(parent)
{
    m_window = new OpenGLWindow(QWidget::width(), QWidget::height(), "");
    m_windowContainer = QWidget::createWindowContainer(m_window, this);
}

void OpenGLWidget::swapBuffers()
{
    return m_window->swapBuffers();
}

Window* OpenGLWidget::window()
{
    return m_window;
}

const Window* OpenGLWidget::window() const
{
    return m_window;
}

}    //  namespace shell::qt
