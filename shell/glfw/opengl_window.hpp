#pragma once

#include "window.hpp"

#include <iopengl_window.hpp>

#include <string>
#include <vector>
#include <functional>

class GLFWwindow;

namespace shell::glfw {

class OpenGLWindow
    : public IOpenGLWindow
    , public Window
{
public:
    OpenGLWindow(int width, int height, std::string name);
    ~OpenGLWindow();

private:
    virtual void init() override;
    virtual void destroy() override;
};

}    //  namespace shell::glfw
