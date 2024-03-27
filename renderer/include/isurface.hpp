#pragma once

#include <irender_target.hpp>

#include <functional>

namespace renderer {

class IGraphicsContext;

class ISurface : public IRenderTarget
{
public:
    virtual ~ISurface() {}

    virtual std::pair<int, int> framebufferSize() const = 0;
    virtual bool available() const = 0;

    virtual void registerCursorPosCallback(std::function<void(double, double)> callback) const = 0;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const = 0;
};

}    //  namespace renderer
