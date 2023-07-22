#include "window.hpp"

#include "GLFW/glfw3.h"

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto obj = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

    for (auto &callback : obj->m_framebufferResizeCallbacks)
    {
        callback(width, height);
    }
}

void Window::windowIconifyCallback(GLFWwindow *window, int flag)
{
    auto obj = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    obj->m_iconified = flag;

    for (auto &callback : obj->m_windowIconifiedCallbacks)
    {
        callback(flag);
    }
}

void Window::onKeyPressedCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto obj = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

    for (auto &callback : obj->m_onKeyPressedCallback)
    {
        callback(key, scancode, action, mods);
    }
}

Window::Window(int width, int height, std::string name)
    : m_name(name)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetKeyCallback(m_window, onKeyPressedCallback);
    glfwSetWindowIconifyCallback(m_window, windowIconifyCallback);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
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
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    return { width, height };
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
