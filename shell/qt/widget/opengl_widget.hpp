#pragma once

#include "widget.hpp"
#include "../window/opengl_window.hpp"

#include <iopengl_surface.hpp>

namespace shell::qt {

class OpenGLWidget
    : public Widget
    , public renderer::IOpenGLSurface
{
public:
    OpenGLWidget(QWidget* parent = nullptr);

    virtual void swapBuffers() override;

private:
    virtual Window* window() override;
    virtual const Window* window() const override;

private:
    OpenGLWindow* m_window;
};

}    //  namespace shell::qt
