#pragma once

#include "specific_buffer.hpp"

#include <iuniform_buffer.hpp>

namespace renderer { namespace vk {

class GraphicsContext;

class UniformBuffer : public SpecificBuffer<IUniformBuffer>
{
public:
    UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo);
    virtual ~UniformBuffer() override;

public:
    virtual void bind(::renderer::OperationContext& context, uint32_t bindingId) const override;

public:
    virtual void write(const void* data, size_t size, size_t offset) override;
    virtual const void* read(size_t size, size_t offset) const override;

    virtual size_t size() const override;
    virtual void reallocate(size_t newSize) override;
};

}}    //  namespace renderer::vk
