#include "window.hpp"

#include <assert.hpp>

#include <QWindow>

namespace shell::qt {

Window::Window(int width, int height, std::string name, QWindow* parent)
    : QWindow(parent)
{
    setWidth(width);
    setHeight(height);
    setTitle(QString::fromStdString(name));
}

Window::~Window() {}

bool Window::event(QEvent* e)
{
    if (e->type() == QEvent::UpdateRequest)
    {
        emit render();
        requestUpdate();
        return true;
    }
    else if (e->type() == QEvent::Close)
    {
        emit aboutToClose();
    }

    return QWindow::event(e);
}

void Window::exposeEvent(QExposeEvent*)
{
    requestUpdate();
}

bool Window::available() const
{
    return !iconified();
}

std::string Window::name() const
{
    return title().toStdString();
}

void Window::close()
{
    QWindow::close();
}

std::pair<int, int> Window::framebufferSize() const
{
    return { QWindow::width(), QWindow::height() };
}

uint32_t Window::width() const
{
    return QWindow::width();
}

uint32_t Window::height() const
{
    return QWindow::height();
}

bool Window::iconified() const
{
    return windowState() == Qt::WindowState::WindowMinimized;
}

void Window::registerCursorPosCallback(std::function<void(double, double)> callback) const {}

void Window::registerFramebufferResizeCallback(std::function<void(int, int)> callback) const {}

void Window::registerWindowIconifiedCallback(std::function<void(bool)> callback) const {}

void Window::registerOnKeyPressedCallback(std::function<void(int, int, int, int)> callback) const {}

}    //  namespace shell::qt
