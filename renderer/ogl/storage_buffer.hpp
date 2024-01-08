#pragma once

#include "ispecific_operation_target.hpp"

#include <istorage_buffer.hpp>

#include <glad/glad.h>

namespace ogl {

class GraphicsContext;

class StorageBuffer : public SpecificOperationTarget<IStorageBuffer>
{
public:
    StorageBuffer(GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);
    virtual ~StorageBuffer();

public:
    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(::OperationContext& context) override;
    virtual void present(::OperationContext& context) override;

    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const override;
    virtual void bind(::OperationContext& context) const override;
    virtual void draw(::OperationContext& context) const override;

    virtual GLuint framebuffer() override;

private:
    GraphicsContext& m_context;

    uint64_t m_elementCount;

    GLuint m_vao;
    GLuint m_buffer;
    std::shared_ptr<IShaderInterfaceHandle> m_handle;
};

}    //  namespace ogl
