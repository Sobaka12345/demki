#pragma once

#include <istorage_buffer.hpp>

namespace ogl {

class GraphicsContext;

class StorageBuffer : public IStorageBuffer
{
public:
    StorageBuffer(GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);
    virtual ~StorageBuffer();

public:
    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(OperationContext& context) override;
    virtual void present(OperationContext& context) override;

    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const override;
    virtual void bind(OperationContext& context) const override;
    virtual void draw(OperationContext& context) const override;

private:
    GraphicsContext& m_context;
};

}    //  namespace ogl
