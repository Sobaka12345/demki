#pragma once

#include "vk/window.hpp"
#include "ogl/window.hpp"

#include <string>


namespace shell::abstract {

struct IWidget : gapi::ISurface
{   
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;

protected:
    ~IWidget() = default;
};

template <typename GApiT>
struct Widget : __private::Window<GApiT, IWidget> {};

}    //  namespace shell::abstract
