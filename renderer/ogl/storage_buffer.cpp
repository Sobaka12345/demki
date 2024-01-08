#include "storage_buffer.hpp"

#include "compute_pipeline.hpp"

#include "graphics_pipeline.hpp"
#include "icomputer.hpp"
#include "shader_interface_handle.hpp"

#include <operation_context.hpp>

namespace ogl {

StorageBuffer::StorageBuffer(GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_elementCount(createInfo.size / createInfo.elementLayoutSize)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, createInfo.size, createInfo.initialData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    m_handle = std::make_shared<StorageBufferInterfaceHandle>(m_buffer);
}

StorageBuffer::~StorageBuffer()
{
    glDeleteBuffers(1, &m_buffer);
    glDeleteVertexArrays(1, &m_vao);
}

void StorageBuffer::accept(ComputerInfoVisitor& visitor) const
{
    visitor.populateComputerInfo(*this);
}

bool StorageBuffer::prepare(::OperationContext& context)
{
    get(context).specificTarget = this;

    return true;
}

void StorageBuffer::present(::OperationContext& context)
{
    auto [x, y, z] = get(context).computePipeline->computeDimensions();

    glDispatchCompute(m_elementCount / x, y, z);
}

std::weak_ptr<IShaderInterfaceHandle> StorageBuffer::handle() const
{
    return m_handle;
}

void StorageBuffer::bind(::OperationContext& context) const
{
    glBindVertexArray(m_vao);
}

void StorageBuffer::draw(::OperationContext& context) const
{
    glDrawArrays(get(context).graphicsPipeline->primitiveTopology(), 0, m_elementCount);
}

GLuint StorageBuffer::framebuffer()
{
    return 0;
}

}    //  namespace ogl
