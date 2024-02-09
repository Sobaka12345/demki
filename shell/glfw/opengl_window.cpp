#include "opengl_window.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <assert.hpp>

namespace shell::glfw {

OpenGLWindow::OpenGLWindow(int width, int height, std::string name)
    : Window(GLFW_OPENGL_API, width, height, name)
{}

OpenGLWindow::~OpenGLWindow() {}

void OpenGLWindow::init()
{
    glfwMakeContextCurrent(glfwWindow());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to initialize opengl");
}

void OpenGLWindow::destroy() {}


}    //  namespace shell::glfw
