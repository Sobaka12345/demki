#pragma once

#include <glad/glad.h>

namespace renderer {

class IOperationTarget;

namespace ogl {

class OperationContext;

class ISpecificOperationTarget
{
public:
    virtual ~ISpecificOperationTarget() {}

    virtual renderer::IOperationTarget* toBase() = 0;
    virtual GLuint framebuffer() = 0;
};

template <typename Base>
class SpecificOperationTarget
    : public ISpecificOperationTarget
    , public Base
{
    virtual Base* toBase() final override { return this; }
};

}    //  namespace ogl
}    //  namespace renderer
