#include "model.hpp"

#include "handles/command_buffer.hpp"

#include "graphics_context.hpp"

namespace vk {

Model::Model(GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_verticesSize(createInfo.vertices.size_bytes())
    , m_indicesSize(createInfo.indices.size_bytes())
    , m_vertexSize(sizeof(createInfo.vertices[0]))
    , m_indexSize(sizeof(createInfo.indices[0]))
{
    m_memory = context.fetchMemory(m_verticesSize + m_indicesSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    auto mapped = m_memory.lock()->map();
    mapped.lock()->write(createInfo.vertices.data(), m_verticesSize, 0);
    mapped.lock()->write(createInfo.indices.data(), m_indicesSize, m_verticesSize);
    mapped.lock()->sync(m_verticesSize + m_indicesSize, 0);
    m_memory.lock()->unmap();
}

void Model::draw(::OperationContext& context)
{
    get(context).commandBuffer->drawIndexed(m_indicesSize / m_indexSize, 1, 0, 0, 0);
}

void Model::bind(::OperationContext& context)
{
    if (m_memory.expired()) return;

    VkDeviceSize offsets[] = { 0 };
    get(context).commandBuffer->bindVertexBuffer(0, 1, m_memory.lock()->buffer().handlePtr(),
        offsets);
    get(context).commandBuffer->bindIndexBuffer(m_memory.lock()->buffer().handle(), m_verticesSize,
        VK_INDEX_TYPE_UINT32);
}

}    //  namespace vk
