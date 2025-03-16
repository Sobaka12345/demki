#include "uniform_buffer.hpp"

#include "graphics_context.hpp"

#include <cstring>

namespace renderer::vk {

using namespace handles;

UniformBuffer::UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo)
    : SpecificBuffer<IUniformBuffer>(context)
{
    allocateBuffer(createInfo.size(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    ASSERT(vkMapMemory(context.device(), m_bufferMemory, 0, m_size, 0, &m_data) == VK_SUCCESS);
}

UniformBuffer::~UniformBuffer()
{
    destroy();
}

void UniformBuffer::bind(::renderer::OperationContext& context, uint32_t bindingId) const {}

void UniformBuffer::write(const void* data, size_t size, size_t offset)
{
    std::memcpy(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(m_data) + offset), data,
        static_cast<size_t>(size));
}

const void* UniformBuffer::read(size_t size, size_t offset) const
{
    return (static_cast<char*>(m_data)) + offset;
}

size_t UniformBuffer::size() const
{
    return m_size;
}

void UniformBuffer::reallocate(size_t newSize)
{
    destroy();
    allocateBuffer(newSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    ASSERT(vkMapMemory(m_context.device(), m_bufferMemory, 0, m_size, 0, &m_data) == VK_SUCCESS);
}

}    //  namespace renderer::vk
