#ifndef GAPI_ISURFACE_HPP
#define GAPI_ISURFACE_HPP

#include "../vk/surface.hpp"
#include "../ogl/surface.hpp"

#include <hierarchy.hpp>

#include <functional>

namespace gapi {

template <typename T>
concept SISurface = requires(
    T& surface,
    const T& constSurface) {
    { surface.handle };
};

struct ISurface
{
    virtual std::pair<int, int> framebufferSize() const = 0;
    virtual bool available() const = 0;
    virtual void waitForEvents() = 0;
    
    virtual void registerCursorPosCallback(std::function<void(double, double)> callback) const = 0;
    virtual void registerFramebufferResizeCallback(
        std::function<void(int, int)> callback) const = 0;

protected:
    ~ISurface() = default;
};

template <typename GApiT>
    requires SISurface<__private::Surface<GApiT>>
struct Surface : __private::Surface<GApiT>
{};

}

#endif // GAPI_ISURFACE_HPP