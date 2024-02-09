#pragma once

#include <iwindow.hpp>

class GLFWwindow;

namespace shell::glfw {

class Window : virtual public IWindow
{
private:
    static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void windowIconifyCallback(GLFWwindow* window, int flag);
    static void onKeyPressedCallback(
        GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    Window(int clientApi, int width, int height, std::string name);
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
        std::function<void(int key, int scancode, int action, int mods)> callback) const override;

    GLFWwindow* glfwHandle();
    const GLFWwindow* glfwHandle() const;

private:
    bool m_iconified;
    int m_width;
    int m_height;
    std::string m_name;
    GLFWwindow* m_window;

    mutable std::vector<std::function<void(double xPos, double yPos)>> m_cursorPosCallback;
    mutable std::vector<std::function<void(int, int)>> m_framebufferResizeCallbacks;
    mutable std::vector<std::function<void(bool)>> m_windowIconifiedCallbacks;
    mutable std::vector<std::function<void(int key, int scancode, int action, int mods)>>
        m_onKeyPressedCallback;
};

}    //  namespace shell::glfw
