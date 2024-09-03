#include "uniform_buffer.hpp"

#include "graphics_context.hpp"
#include "pipeline.hpp"
#include "shader_interface_handle.hpp"

namespace renderer { namespace vk {

UniformBuffer::UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo)
    : m_context(context)
    , m_createInfo(std::move(createInfo))
    , m_handle(context.fetchHandleSpecific(m_createInfo.isDynamic ?
              ShaderBlockType::UNIFORM_DYNAMIC :
              ShaderBlockType::UNIFORM_STATIC,
          m_createInfo.size))
{}

void UniformBuffer::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    if (m_createInfo.isDynamic)
    {
        get(context).pipelineBindContext->dynamicOffsets.push_back(
            m_handle->currentDescriptor()->dynamicOffset);
    }
}

void UniformBuffer::adapt(renderer::OperationContext& context, uint32_t bindingId)
{
    const auto writes = descriptorSetWrites(get(context), bindingId, *m_handle);

    vkUpdateDescriptorSets(m_context.device(), static_cast<uint32_t>(writes.size()), writes.data(),
        0, nullptr);
}

void UniformBuffer::write(const void* data, size_t size)
{
    m_handle->write(data, size);
}

const void* UniformBuffer::read(size_t size) const
{
    return m_handle->read(size);
}

}}    //  namespace renderer::vk
