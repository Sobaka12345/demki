#include "uniform_buffer.hpp"

#include "graphics_context.hpp"
#include "pipeline.hpp"

namespace renderer { namespace ogl {

UniformBuffer::UniformBuffer(GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_size(createInfo.size)
{
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferData(GL_UNIFORM_BUFFER, m_size, NULL,
        createInfo.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_mapped = std::malloc(m_size);
}

void UniformBuffer::write(const void* data, size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    m_invalidated = true;
}

const void* UniformBuffer::read(size_t size) const
{
    if (m_invalidated)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, size, m_mapped);
        m_invalidated = false;
    }

    return m_mapped;
}

void UniformBuffer::adapt(renderer::OperationContext& context, uint32_t bindingId) {}

void UniformBuffer::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingId, m_buffer, 0, m_size);
}

}}    //  namespace renderer::ogl
