#pragma once

#include <iuniform_buffer.hpp>

#include "handles/buffer.hpp"
#include "handles/memory.hpp"

namespace renderer { namespace vk {

class GraphicsContext;

class UniformBuffer : public IUniformBuffer
{
public:
    UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo);
    virtual ~UniformBuffer() override;

public:
    virtual void bind(::renderer::OperationContext& context, uint32_t bindingId) const override;

public:
    virtual void write(const void* data, size_t size, size_t offset) override;
    virtual const void* read(size_t size, size_t offset) const override;

private:
    GraphicsContext& m_context;
    CreateInfo m_createInfo;

    void* m_data;
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
};

}}    //  namespace renderer::vk
