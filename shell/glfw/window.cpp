#include "window.hpp"

#include <assert.hpp>

#include <GLFW/glfw3.h>

namespace shell::glfw {

namespace __private {

void BaseGlfwWindow::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
    auto obj = reinterpret_cast<BaseGlfwWindow*>(glfwGetWindowUserPointer(window));

    for (auto& callback : obj->m_cursorPosCallback)
    {
        callback(xPos, yPos);
    }
}

void BaseGlfwWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto obj = reinterpret_cast<BaseGlfwWindow*>(glfwGetWindowUserPointer(window));
    obj->m_width = width;
    obj->m_height = height;

    for (auto& callback : obj->m_framebufferResizeCallbacks)
    {
        callback(width, height);
    }
}

void BaseGlfwWindow::windowIconifyCallback(GLFWwindow* window, int flag)
{
    auto obj = reinterpret_cast<BaseGlfwWindow*>(glfwGetWindowUserPointer(window));
    obj->m_iconified = flag;

    for (auto& callback : obj->m_windowIconifiedCallbacks)
    {
        callback(flag);
    }
}

void BaseGlfwWindow::onKeyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto obj = reinterpret_cast<BaseGlfwWindow*>(glfwGetWindowUserPointer(window));

    for (auto& callback : obj->m_onKeyPressedCallback)
    {
        callback(key, scancode, action, mods);
    }
}

BaseGlfwWindow::BaseGlfwWindow(int clientApi, int width, int height, std::string name)
    : m_iconified(false)
    , m_width(width)
    , m_height(height)
    , m_name(name)
    , m_window(nullptr)
{
    glfwWindowHint(GLFW_CLIENT_API, clientApi);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

BaseGlfwWindow::~BaseGlfwWindow()
{
    if (m_window) glfwDestroyWindow(m_window);
}

bool BaseGlfwWindow::available() const
{
    return !m_iconified;
}

std::string BaseGlfwWindow::name() const
{
    return m_name;
}

void BaseGlfwWindow::close()
{
    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool BaseGlfwWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

std::pair<int, int> BaseGlfwWindow::framebufferSize() const
{
    return { m_width, m_height };
}

uint32_t BaseGlfwWindow::width() const
{
    return m_width;
}

uint32_t BaseGlfwWindow::height() const
{
    return m_height;
}

void BaseGlfwWindow::registerFramebufferResizeCallback(std::function<void(int, int)> callback) const
{
    m_framebufferResizeCallbacks.push_back(callback);
}

void BaseGlfwWindow::registerWindowIconifiedCallback(std::function<void(bool)> callback) const
{
    m_windowIconifiedCallbacks.push_back(callback);
}

void BaseGlfwWindow::registerOnKeyPressedCallback(
    std::function<void(int key, int scancode, int action, int mods)> callback) const
{
    m_onKeyPressedCallback.push_back(callback);
}

GLFWwindow* BaseGlfwWindow::glfwHandle()
{
    return m_window;
}

const GLFWwindow* BaseGlfwWindow::glfwHandle() const
{
    return m_window;
}

GLFWwindow* BaseGlfwWindow::create()
{
    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetKeyCallback(m_window, onKeyPressedCallback);
    glfwSetWindowIconifyCallback(m_window, windowIconifyCallback);

    return m_window;
}

bool BaseGlfwWindow::iconified() const
{
    return m_iconified;
}

void BaseGlfwWindow::waitForEvents()
{
    glfwWaitEvents();
}

}

}    //  namespace shell::glfw
