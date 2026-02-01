#pragma once

#include "vk/window.hpp"
#include "ogl/window.hpp"

#include <string>


namespace shell::abstract {

template <typename T>
concept SIWindow = requires(
    T& window,
    const T& constwindow) {
    { window };
};

struct IWindow : gapi::ISurface
{
    virtual std::string name() const = 0;
    virtual void close() = 0;
    virtual bool iconified() const = 0;
    
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;

    virtual void registerWindowIconifiedCallback(std::function<void(bool)> callback) const = 0;
    virtual void registerOnKeyPressedCallback(
        std::function<void(int key, int scancode, int action, int mods)> callback) const = 0;

protected:
    ~IWindow() = default;
};

template <typename GApiT>
    requires SIWindow<__private::Window<GApiT>>
struct Window : __private::Window<GApiT> {};

}    //  namespace shell::abstract
