#pragma once

#include "../abstract/window.hpp"
#include "shell_fwd.hpp"
#include "type_list.hpp"
#include "vk/window.hpp"

#include <hierarchy.hpp>

#include <QWindow>

namespace shell::qt {

namespace __private {

class BaseQWindow: public QWindow, public abstract::IWindow {
    Q_OBJECT

public:
    BaseQWindow(int width, int height, std::string name, QWindow* parent = nullptr);
    virtual ~BaseQWindow() override;

    // QWindow
    virtual bool event(QEvent* e) override;
    virtual bool eventFilter(QObject* watched, QEvent* e) override;
    virtual void exposeEvent(QExposeEvent*) override;

    // IWindow
    virtual bool available() const override { return !iconified(); }
    virtual std::string name() const override { return QWindow::title().toStdString(); }
    virtual void close() override { QWindow::close(); }
    virtual std::pair<int, int> framebufferSize() const override { return { QWindow::width(), QWindow::height() }; }
    virtual uint32_t width() const override { return QWindow::width(); }
    virtual uint32_t height() const override { return QWindow::height(); }
    virtual bool iconified() const override { return QWindow::windowState() == Qt::WindowState::WindowMinimized; }
    virtual void waitForEvents() override;

signals:
    void render();
    void aboutToOpen();
    void aboutToClose();
    void iconifiedChanged(bool iconified);
    void sizeChanged(int width, int height);
    void mousePosChanged(float posX, float posY);
};

template <typename GApiT>
using BaseWindowHierarchy = ComplexLinearHierarchy<TypeList<GApiT>, 
    NonLinearHierarchy<TypeList<GApiT>,
        abstract::Window, 
        BaseQWindow
    >, 
    Window>;
}

template <typename GApiT>
class Window :  public __private::BaseWindowHierarchy<GApiT>
{
public:
    using __private::BaseWindowHierarchy<GApiT>::BaseWindowHierarchy;

    // REMOVE these callback registrations
    void registerCursorPosCallback(std::function<void(double, double)> callback) const
    {
        m_cursorPosCallback.push_back(callback);
    }

    void registerFramebufferResizeCallback(std::function<void(int, int)> callback) const
    {
        m_framebufferResizeCallbacks.push_back(callback);
    }

    void registerWindowIconifiedCallback(std::function<void(bool)> callback) const
    {
        m_windowIconifiedCallbacks.push_back(callback);
    }

    void registerOnKeyPressedCallback(std::function<void(int, int, int, int)> callback) const
    {
        m_onKeyPressedCallback.push_back(callback);
    }

private:
    mutable std::vector<std::function<void(double xPos, double yPos)>> m_cursorPosCallback;
    mutable std::vector<std::function<void(int, int)>> m_framebufferResizeCallbacks;
    mutable std::vector<std::function<void(bool)>> m_windowIconifiedCallbacks;
    mutable std::vector<std::function<void(int key, int scancode, int action, int mods)>>
        m_onKeyPressedCallback;
};

}    //  namespace shell::qt
