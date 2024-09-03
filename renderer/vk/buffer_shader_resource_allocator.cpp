#include "buffer_shader_resource_allocator.hpp"


#include "graphics_context.hpp"
#include "shader_interface_handle.hpp"

#include <ranges>

namespace renderer::vk {

BufferShaderResourceAllocator::BufferShaderResourceAllocator(
    GraphicsContext& context, uint32_t alignment, uint32_t chunkObjectCount)
    : m_chunkObjectCount(chunkObjectCount)
    , m_alignment(alignment)
    , m_context(context)
{}

std::shared_ptr<ShaderResourceAllocator::Descriptor> BufferShaderResourceAllocator::
    fetchDescriptor()
{
    for (int64_t bufferId = m_buffers.size() - 1; bufferId >= 0; --bufferId)
    {
        if (auto result = tryFetchDescriptor(bufferId); result)
        {
            return result;
        }
    }

    if (auto result = tryFetchDescriptor(allocateBuffer()); result)
    {
        return result;
    }

    ASSERT(false, "failed to allocate uniform object");
    return nullptr;
}

size_t BufferShaderResourceAllocator::allocateBuffer()
{
    auto& newBuffer = m_buffers.emplaceBack(m_context.device(), bufferCreateInfo());
    newBuffer.allocateAndBindMemory(memoryProperties()).lock()->map();
    m_freeDescriptors.push_back(std::unordered_set<uint64_t>{});
    auto& freeSet = m_freeDescriptors.back();

    for (uint32_t val : std::views::iota(static_cast<uint32_t>(0), m_chunkObjectCount))
    {
        freeSet.insert(val);
    }

    return m_buffers.size() - 1;
}

void BufferShaderResourceAllocator::populateDescriptor(Descriptor& descriptor)
{
    auto& buffer = m_buffers[descriptor.id.bufferId];
    descriptor.descriptorBufferInfo =
        DescriptorBufferInfo{}
            .buffer(buffer)
            .offset(descriptor.id.descriptorId * alignment())
            .range(m_alignment);
    descriptor.memory = buffer.memory();
}

std::shared_ptr<ShaderResourceAllocator::Descriptor> BufferShaderResourceAllocator::
    tryFetchDescriptor(size_t bufferId)
{
    if (auto& freeSet = m_freeDescriptors[bufferId]; !freeSet.empty())
    {
        uint64_t freeDescriptorId = freeSet.extract(freeSet.begin()).value();

        auto descriptor = ShaderResourceAllocator::fetchDescriptor();

        descriptor->id.descriptorId = freeDescriptorId;
        descriptor->id.bufferId = bufferId;
        descriptor->id.resourceId = id();

        populateDescriptor(*descriptor);

        return descriptor;
    }

    return nullptr;
}

void BufferShaderResourceAllocator::freeDescriptor(
    const ShaderResourceAllocator::Descriptor& descriptor)
{
    m_freeDescriptors[descriptor.id.bufferId].insert(descriptor.id.descriptorId);
}

handles::BufferCreateInfo UniformBufferShaderResourceAllocator::bufferCreateInfo() const
{
    return handles::BufferCreateInfo{}
        .size(m_alignment * m_chunkObjectCount)
        .usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

VkMemoryPropertyFlags UniformBufferShaderResourceAllocator::memoryProperties() const
{
    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

void DynamicUniformBufferShaderResourceAllocator::populateDescriptor(Descriptor& descriptor)
{
    auto& buffer = m_buffers[descriptor.id.bufferId];
    descriptor.descriptorBufferInfo =
        DescriptorBufferInfo{}.buffer(buffer).offset(0).range(m_alignment);
    descriptor.dynamicOffset = descriptor.id.descriptorId * alignment();
    descriptor.memory = buffer.memory();
}

void StaticUniformBufferShaderResourceAllocator::populateDescriptor(Descriptor& descriptor)
{
    UniformBufferShaderResourceAllocator::populateDescriptor(descriptor);
}

handles::BufferCreateInfo StorageBufferShaderResourceAllocator::bufferCreateInfo() const
{
    return handles::BufferCreateInfo{}
        .size(m_alignment * m_chunkObjectCount)
        .usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

VkMemoryPropertyFlags StorageBufferShaderResourceAllocator::memoryProperties() const
{
    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}


}    //  namespace renderer::vk
