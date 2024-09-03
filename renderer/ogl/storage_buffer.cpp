#include "storage_buffer.hpp"

#include "graphics_context.hpp"
#include "compute_pipeline.hpp"

#include "utils.hpp"
#include "graphics_pipeline.hpp"
#include "icomputer.hpp"

#include <operation_context.hpp>

namespace renderer::ogl {

StorageBuffer::StorageBuffer(GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_elementCount(createInfo.initialDataSize)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);

    glBindVertexArray(m_vao);

    auto& dataMetaInfo = createInfo.dataTypeMetaInfo;
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_elementCount * dataMetaInfo.typeSize, createInfo.initialData,
        GL_STATIC_DRAW);

    size_t j = 0;
    for (auto& field : dataMetaInfo.fields)
    {
        auto [dimensionCount, glType] = toGLCompoundTypeSize(field.typeId);
        glVertexAttribPointer(j, dimensionCount, glType, createInfo.normalized ? GL_TRUE : GL_FALSE,
            dataMetaInfo.typeSize, reinterpret_cast<void*>(field.shift));
        glEnableVertexAttribArray(j++);
    }
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

bool StorageBuffer::prepare(renderer::OperationContext& context)
{
    get(context).specificTarget = this;

    return true;
}

void StorageBuffer::present(renderer::OperationContext& context)
{
    auto [x, y, z] = get(context).computePipeline->computeDimensions();

    glDispatchCompute(m_elementCount / x, y, z);
}

void StorageBuffer::draw(renderer::OperationContext& context) const
{
    glBindVertexArray(m_vao);
    glDrawArrays(get(context).graphicsPipeline->primitiveTopology(), 0, m_elementCount);
}

GLuint StorageBuffer::framebuffer()
{
    return 0;
}

void StorageBuffer::adapt(renderer::OperationContext& context, uint32_t bindingId)
{
    //  NOTHING TO DO
}

void StorageBuffer::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingId, m_buffer);
}


}    //  namespace renderer::ogl
