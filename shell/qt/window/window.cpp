#include "window.hpp"

#include <assert.hpp>

#include <QWindow>
#include <QWindowStateChangeEvent>
#include <QResizeEvent>
#include <QMouseEvent>

namespace shell::qt {

Window::Window(int width, int height, std::string name, QWindow* parent)
    : QWindow(parent)
{
    setWidth(width);
    setHeight(height);
    setTitle(QString::fromStdString(name));

    installEventFilter(this);
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
    else if (e->type() == QEvent::Resize)
    {
        //  auto event = static_cast<QResizeEvent*>(e);
        //  for (auto& callback : m_framebufferResizeCallbacks)
        //  {
        //      callback(event->size().width(), event->size().height());
        //  }
    }
    else if (e->type() == QEvent::WindowStateChange)
    {
        auto event = static_cast<QWindowStateChangeEvent*>(e);
        if (auto iconified = windowState() == Qt::WindowMinimized;
            iconified || event->oldState() == Qt::WindowMinimized)
            for (auto& callback : m_windowIconifiedCallbacks)
            {
                callback(iconified);
            }
    }
    else if (e->type() == QEvent::KeyPress)
    {
        //  not implemented
    }
    else if (e->type() == QEvent::MouseMove)
    {
        auto event = static_cast<QMouseEvent*>(e);
        auto pos = event->pos().toPointF();
        for (auto& callback : m_cursorPosCallback)
        {
            callback(pos.x(), pos.y());
        }
    }
    else if (e->type() == QEvent::Close)
    {
        emit aboutToClose();
    }

    return QWindow::event(e);
}

bool Window::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::Resize && watched == this)
    {
        auto event = static_cast<QResizeEvent*>(e);
        for (auto& callback : m_framebufferResizeCallbacks)
        {
            callback(event->size().width(), event->size().height());
        }
    }

    return QWindow::eventFilter(watched, e);
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

void Window::registerCursorPosCallback(std::function<void(double, double)> callback) const
{
    m_cursorPosCallback.push_back(callback);
}

void Window::registerFramebufferResizeCallback(std::function<void(int, int)> callback) const
{
    m_framebufferResizeCallbacks.push_back(callback);
}

void Window::registerWindowIconifiedCallback(std::function<void(bool)> callback) const
{
    m_windowIconifiedCallbacks.push_back(callback);
}

void Window::registerOnKeyPressedCallback(std::function<void(int, int, int, int)> callback) const
{
    m_onKeyPressedCallback.push_back(callback);
}

}    //  namespace shell::qt
