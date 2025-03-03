#include "uniform_buffer.hpp"

#include "graphics_context.hpp"

#include <cstring>

namespace renderer::vk {

using namespace handles;

UniformBuffer::UniformBuffer(GraphicsContext& context, IUniformBuffer::CreateInfo createInfo)
    : m_context(context)
    , m_createInfo(std::move(createInfo))
{
    auto bufferCreateInfo =
        BufferCreateInfo{}
            .size(createInfo.size())
            .usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
    m_buffer = Buffer::create(context.device(), &bufferCreateInfo, nullptr);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context.device(), m_buffer, &memRequirements);

    //  TO DO: Change memory types
    const auto properties =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    const auto memAllocInfo =
        MemoryAllocateInfo{}
            .allocationSize(memRequirements.size)
            .memoryTypeIndex(DeviceMemory::findMemoryType(context.physicalDevice(),
                memRequirements.memoryTypeBits, properties));
    m_memory = DeviceMemory::create(context.device(), &memAllocInfo, nullptr);

    ASSERT(vkMapMemory(context.device(), m_memory, 0, createInfo.size(), 0, &m_data) == VK_SUCCESS);
}

UniformBuffer::~UniformBuffer()
{
    DeviceMemory::destroy(m_context.device(), m_memory, nullptr);
    Buffer::destroy(m_context.device(), m_buffer, nullptr);
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

}    //  namespace renderer::vk
