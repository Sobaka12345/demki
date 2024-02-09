#pragma once

#include "window.hpp"

#include <iopengl_window.hpp>

class QOpenGLWindow;

namespace shell::qt {

class OpenGLWindow
    : public IOpenGLWindow
    , public Window
{
public:
    OpenGLWindow(int width, int height, std::string name, QWindow* parent = nullptr);
    ~OpenGLWindow();

    virtual QWindow* qWindow() override;
    virtual const QWindow* qWindow() const override;

private:
    virtual void init() override;
    virtual void destroy() override;

private:
    QOpenGLWindow* m_window;
};

}    //  namespace shell::qt
