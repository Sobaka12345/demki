#pragma once

#include "iwindow.hpp"

namespace renderer::ogl {
class GraphicsContext;
}

namespace shell {

class IOpenGLWindow : virtual public IWindow
{
private:
    virtual void init() = 0;
    virtual void destroy() = 0;

    friend class renderer::ogl::GraphicsContext;
};

}
