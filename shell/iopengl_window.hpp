#pragma once

#include "iwindow.hpp"

#include <iopengl_surface.hpp>

namespace shell {

class IOpenGLWindow
    : virtual public IWindow
    , public renderer::IOpenGLSurface
{};

}
