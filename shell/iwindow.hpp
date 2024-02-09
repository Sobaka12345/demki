#pragma once

#include <string>
#include <functional>


enum class GAPI
{
    OpenGL,
    Vulkan
};

namespace shell {

class IWindow
{
public:
    virtual ~IWindow() {}

    virtual std::string name() const = 0;

    virtual void swapBuffers() = 0;
    virtual void close() = 0;
    virtual bool shouldClose() const = 0;

    virtual std::pair<int, int> framebufferSize() const = 0;
    virtual bool iconified() const = 0;

    virtual void registerCursorPosCallback(std::function<void(double, double)> callback) const = 0;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const = 0;
    virtual void registerWindowIconifiedCallback(std::function<void(bool)> callback) const = 0;
    virtual void registerOnKeyPressedCallback(
        std::function<void(int key, int scancode, int action, int mods)> callback) const = 0;
};

}    //  namespace shell
