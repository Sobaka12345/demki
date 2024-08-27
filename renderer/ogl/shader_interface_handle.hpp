#pragma once

#include <iuniform_buffer.hpp>
#include <ishader_interface_handle.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

struct ShaderInterfaceHandle
    : public IShaderInterfaceHandle
    , public std::enable_shared_from_this<ShaderInterfaceHandle>
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

    //  TO DO: remove or unify this nonsense
    virtual void reset(GLuint resourceId) {};
};

struct UniformBufferInterfaceHandle
    : public ShaderInterfaceHandle
    , public IUniformBuffer
{
private:
    explicit UniformBufferInterfaceHandle(size_t size, GLenum memoryUsage)
        : size(size)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, memoryUsage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        mapped = std::malloc(size);
    }

public:
    [[nodiscard]] static std::shared_ptr<UniformBufferInterfaceHandle> create(size_t size,
        GLenum memoryUsage)
    {
        return std::shared_ptr<UniformBufferInterfaceHandle>{ new UniformBufferInterfaceHandle(size,
            memoryUsage) };
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

    virtual std::shared_ptr<IShaderInterfaceHandle> handle() override { return shared_from_this(); }

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
private:
    explicit TextureInterfaceHandle(GLuint texture = 0)
        : texture(texture)
    {}

public:
    [[nodiscard]] static std::shared_ptr<TextureInterfaceHandle> create(GLuint texture = 0)
    {
        return std::shared_ptr<TextureInterfaceHandle>{ new TextureInterfaceHandle(texture) };
    }

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

    virtual void reset(GLuint id) override { texture = id; }

    GLuint texture;
};

struct StorageBufferInterfaceHandle : public ShaderInterfaceHandle
{
    explicit StorageBufferInterfaceHandle(GLuint storageBuffer = 0)
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

    virtual void reset(GLuint id) override { storageBuffer = id; }

    GLuint storageBuffer;
};

}    //  namespace renderer::ogl
