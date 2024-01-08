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

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const override
    {
        visitor.visit(*this);
    }

    virtual void bind(GLuint binding) = 0;
};

struct UniformBufferInterfaceHandle : public ShaderInterfaceHandle
{
    explicit UniformBufferInterfaceHandle(size_t size, GLenum memoryUsage)
        : size(size)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, memoryUsage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        mapped = std::malloc(size);
    }

    ~UniformBufferInterfaceHandle()
    {
        if (mapped)
        {
            std::free(mapped);
        }
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

    virtual void bind(GLuint binding) override
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer, 0, size);
    }

    void* mapped = nullptr;
    mutable bool invalidated = false;
    size_t size;
    GLuint buffer;
};

struct TextureInterfaceHandle : public ShaderInterfaceHandle
{
    explicit TextureInterfaceHandle(GLuint texture)
        : texture(texture)
    {}

    virtual void write(const void* src, size_t size) override { ASSERT(false, "not implemented"); }

    virtual const void* read(size_t size) const override
    {
        ASSERT(false, "not implemented");
        return nullptr;
    }

    virtual void bind(GLuint binding) override
    {
        glActiveTexture(GL_TEXTURE0 + binding);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    GLuint texture;
};

struct StorageBufferInterfaceHandle : public ShaderInterfaceHandle
{
    explicit StorageBufferInterfaceHandle(GLuint storageBuffer)
        : storageBuffer(storageBuffer)
    {}

    virtual void write(const void* src, size_t size) override { ASSERT(false, "not implemented"); }

    virtual const void* read(size_t size) const override
    {
        ASSERT(false, "not implemented");
        return nullptr;
    }

    virtual void bind(GLuint binding) override
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, storageBuffer);
    }

    GLuint storageBuffer;
};

}    //  namespace ogl
