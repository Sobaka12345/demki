#pragma once

#include <iuniform_buffer.hpp>

#include <glad/glad.h>

namespace renderer { namespace ogl {

class GraphicsContext;

class UniformBuffer : public IUniformBuffer
{
public:
    UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo);

public:
    //  IBuffer interface
    virtual void write(const void* data, size_t size) override;
    virtual const void* read(size_t size) const override;

    //  IShaderResource interface
    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) override;
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;

private:
    GraphicsContext& m_context;

    void* m_mapped = nullptr;
    size_t m_size;
    GLuint m_buffer;
    mutable bool m_invalidated = false;
};

}}    //  namespace renderer::ogl
