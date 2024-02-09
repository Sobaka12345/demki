#include "window.hpp"

#include <QWindow>

namespace shell::qt {

Window::Window() {}

Window::~Window() {}

std::string Window::name() const
{
    return qWindow()->title().toStdString();
}

void Window::swapBuffers() {}

void Window::close()
{
    qWindow()->close();
}

bool Window::shouldClose() const {}

std::pair<int, int> Window::framebufferSize() const
{
    return { qWindow()->size().width(), qWindow()->size().height() };
}

bool Window::iconified() const
{
    return qWindow()->windowState() == Qt::WindowState::WindowMinimized;
}

void Window::registerCursorPosCallback(std::function<void(double, double)> callback) const {}

void Window::registerFramebufferResizeCallback(std::function<void(int, int)> callback) const {}

void Window::registerWindowIconifiedCallback(std::function<void(bool)> callback) const {}

void Window::registerOnKeyPressedCallback(std::function<void(int, int, int, int)> callback) const {}


}    //  namespace shell::qt
