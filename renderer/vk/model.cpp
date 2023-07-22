#include "model.hpp"

namespace vk {

Model::Model(Descriptor descriptor)
    : m_descriptor(std::move(descriptor))
{}

void Model::draw(const ::RenderContext& context)
{
    get(context).commandBuffer->drawIndexed(m_descriptor.indices.objectCount(), 1, 0, 0, 0);
}

void Model::bind(const ::RenderContext& context)
{
    VkDeviceSize offsets[] = { m_descriptor.vertices.offset };
    get(context).commandBuffer->bindVertexBuffer(0, 1, m_descriptor.vertices.buffer.handlePtr(),
        offsets);
    get(context).commandBuffer->bindIndexBuffer(m_descriptor.indices.buffer,
        m_descriptor.indices.offset, VK_INDEX_TYPE_UINT32);
}

}    //  namespace vk
