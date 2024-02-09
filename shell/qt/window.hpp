#pragma once

#include <iwindow.hpp>

class QWindow;

namespace shell::qt {

class Window : virtual public IWindow
{
public:
    Window();
    ~Window();

    virtual std::string name() const override;
    virtual void swapBuffers() override;
    virtual void close() override;
    virtual bool shouldClose() const override;
    virtual std::pair<int, int> framebufferSize() const override;
    virtual bool iconified() const override;
    virtual void registerCursorPosCallback(
        std::function<void(double, double)> callback) const override;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const override;
    virtual void registerWindowIconifiedCallback(std::function<void(bool)> callback) const override;
    virtual void registerOnKeyPressedCallback(
        std::function<void(int, int, int, int)> callback) const override;

    virtual QWindow* qWindow() = 0;
    virtual const QWindow* qWindow() const = 0;
};

}    //  namespace shell::qt
