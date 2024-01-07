#pragma once

#include <ishader_interface_handle.hpp>

#include <glad/glad.h>

namespace ogl {

struct ShaderInterfaceHandle : public IShaderInterfaceHandle
{
    struct TypeVisitor : public ShaderInterfaceHandleVisitor
    {
        void visit(ShaderInterfaceHandle& handle) override { this->handle = &handle; }

        ShaderInterfaceHandle* operator->() { return handle; }

    private:
        ShaderInterfaceHandle* handle = nullptr;
    };

    explicit ShaderInterfaceHandle(size_t size, GLenum memoryUsage)
        : size(size)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, memoryUsage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        mapped = std::malloc(size);
    }

    explicit ShaderInterfaceHandle(GLenum texture)
        : texture(texture)
    {}

    ~ShaderInterfaceHandle()
    {
        if (mapped)
        {
            std::free(mapped);
        }
    }

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const override
    {
        visitor.visit(*this);
    }

    virtual void write(const void* src, size_t size) override
    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, src);
        invalidated = true;
    }

    virtual const void* read(size_t size) const override
    {
        if (invalidated)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, buffer);
            glGetBufferSubData(GL_UNIFORM_BUFFER, 0, size, mapped);
            invalidated = false;
        }
        return mapped;
    }

    void* mapped = nullptr;
    mutable bool invalidated = false;
    size_t size;
    GLuint buffer;
    GLuint texture;
};

}    //  namespace ogl
