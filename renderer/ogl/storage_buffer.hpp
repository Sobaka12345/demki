#pragma once

#include "ispecific_operation_target.hpp"

#include <istorage_buffer.hpp>

#include <glad/glad.h>

#include <boost/pfr.hpp>

namespace ogl {

class GraphicsContext;

class StorageBuffer : public SpecificOperationTarget<IStorageBuffer>
{
public:
    StorageBuffer(GraphicsContext& context, CreateInfo createInfo);
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
    void init(const void* data, size_t sizeInBytes, size_t typeSize);

private:
    GraphicsContext& m_context;

    uint64_t m_elementCount;

    GLuint m_vao;
    GLuint m_buffer;
    std::shared_ptr<IShaderInterfaceHandle> m_handle;
};

}    //  namespace ogl
