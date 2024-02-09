#include "window.hpp"

#include <assert.hpp>

#include <GLFW/glfw3.h>

namespace shell::glfw {

void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    for (auto& callback : obj->m_cursorPosCallback)
    {
        callback(xPos, yPos);
    }
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    obj->m_width = width;
    obj->m_height = height;

    for (auto& callback : obj->m_framebufferResizeCallbacks)
    {
        callback(width, height);
    }
}

void Window::windowIconifyCallback(GLFWwindow* window, int flag)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    obj->m_iconified = flag;

    for (auto& callback : obj->m_windowIconifiedCallbacks)
    {
        callback(flag);
    }
}

void Window::onKeyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    for (auto& callback : obj->m_onKeyPressedCallback)
    {
        callback(key, scancode, action, mods);
    }
}

Window::Window(int clientApi, int width, int height, std::string name)
    : m_name(name)
    , m_width(width)
    , m_height(height)
    , m_window(nullptr)
    , m_iconified(false)
{
    glfwWindowHint(GLFW_CLIENT_API, clientApi);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(width, height, m_name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetKeyCallback(m_window, onKeyPressedCallback);
    glfwSetWindowIconifyCallback(m_window, windowIconifyCallback);
}

Window::~Window()
{
    if (m_window) glfwDestroyWindow(m_window);
}

std::string Window::name() const
{
    return m_name;
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::close()
{
    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

std::pair<int, int> Window::framebufferSize() const
{
    return { m_width, m_height };
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

void Window::registerOnKeyPressedCallback(
    std::function<void(int key, int scancode, int action, int mods)> callback) const
{
    m_onKeyPressedCallback.push_back(callback);
}

GLFWwindow* Window::glfwWindow()
{
    return m_window;
}

const GLFWwindow* Window::glfwWindow() const
{
    return m_window;
}

bool Window::iconified() const
{
    return m_iconified;
}

}    //  namespace shell::glfw
