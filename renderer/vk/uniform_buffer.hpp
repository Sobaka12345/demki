#pragma once

#include "specific_shader_resource.hpp"

#include <iuniform_buffer.hpp>

namespace renderer { namespace vk {

class GraphicsContext;

class UniformBuffer : public SpecificShaderResource<IUniformBuffer>
{
public:
    UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo);

public:
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;
    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) override;

public:
    virtual void write(const void* data, size_t size) override;
    virtual const void* read(size_t size) const override;

private:
    GraphicsContext& m_context;
    CreateInfo m_createInfo;
    std::shared_ptr<ShaderInterfaceHandle> m_handle;
};

}}    //  namespace renderer::vk
