#pragma once

#include "isurface.hpp"

#include <string>

namespace shell {

class IWindow : virtual public renderer::ISurface
{
public:
    virtual std::string name() const = 0;

    virtual void close() = 0;

    virtual bool iconified() const = 0;

    virtual renderer::IGraphicsContext& graphicsContext() = 0;

    virtual void registerWindowIconifiedCallback(std::function<void(bool)> callback) const = 0;
    virtual void registerOnKeyPressedCallback(
        std::function<void(int key, int scancode, int action, int mods)> callback) const = 0;
};

}    //  namespace shell
