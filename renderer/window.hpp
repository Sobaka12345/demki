#pragma once

#include <iwindow.hpp>

#include <string>
#include <vector>
#include <functional>

class GLFWwindow;

class Window : public IWindow
{
private:
    static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void windowIconifyCallback(GLFWwindow* window, int flag);
    static void onKeyPressedCallback(
        GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    Window(int width, int height, std::string name);
    ~Window();

    void close();

    std::string name() const { return m_name; }

    bool iconified() const { return m_iconified; }

    GLFWwindow* glfwHandle() const { return m_window; }

    bool shouldClose() const;
    std::pair<int, int> framebufferSize() const;

    void registerCursorPosCallback(std::function<void(double, double)> callback) const;
    void registerFramebufferResizeCallback(std::function<void(int, int)> callback) const;
    void registerWindowIconifiedCallback(std::function<void(bool)> callback) const;
    void registerOnKeyPressedCallback(
        std::function<void(int key, int scancode, int action, int mods)> callback) const;

private:
    virtual void init(GAPI gapi) override;

private:
    bool m_iconified;

    mutable std::vector<std::function<void(double xPos, double yPos)>> m_cursorPosCallback;
    mutable std::vector<std::function<void(int, int)>> m_framebufferResizeCallbacks;
    mutable std::vector<std::function<void(bool)>> m_windowIconifiedCallbacks;
    mutable std::vector<std::function<void(int key, int scancode, int action, int mods)>>
        m_onKeyPressedCallback;

    int m_width, m_height;
    std::string m_name;
    GLFWwindow* m_window;
};
