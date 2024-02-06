#include "model.hpp"
#include "graphics_pipeline.hpp"

#include <operation_context.hpp>

namespace ogl {

Model::Model(GraphicsContext& context, CreateInfo createInfo) noexcept
    : m_context(context)
    , m_indexCount(createInfo.indices.size())
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vertexBuffer);
    glGenBuffers(1, &m_indexBuffer);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, createInfo.vertices.size() * sizeof(createInfo.vertices[0]),
        createInfo.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo.indices.size() * sizeof(createInfo.indices[0]),
        createInfo.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //  color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //  texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

Model::~Model()
{
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vao);
}

void Model::bind(::OperationContext& context)
{
    glBindVertexArray(m_vao);
}

void Model::draw(::OperationContext& context)
{
    glDrawElements(get(context).graphicsPipeline->primitiveTopology(), m_indexCount,
        GL_UNSIGNED_INT, 0);
}

}    //  namespace ogl
