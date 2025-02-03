#pragma once

#include "isurface.hpp"
#include "../ogl/graphics_context.hpp"

namespace renderer {

class IGraphicsContext;

class IOpenGLSurface : virtual public ISurface
{
public:
    virtual void swapBuffers() = 0;

protected:
    ogl::GraphicsContext* createContext() { return new ogl::GraphicsContext(*this); }
};

}    //  namespace renderer
