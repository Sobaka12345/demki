#include "storage_buffer.hpp"

#include "compute_pipeline.hpp"

#include "utils.hpp"
#include "graphics_pipeline.hpp"
#include "icomputer.hpp"
#include "shader_interface_handle.hpp"

#include <operation_context.hpp>

namespace ogl {

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
