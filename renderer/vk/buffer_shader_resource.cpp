#include "buffer_shader_resource.hpp"

#include <ranges>

namespace vk {

BufferShaderResource::BufferShaderResource(
    const handles::Device& device, uint32_t alignment, uint32_t chunkObjectCount)
    : m_chunkObjectCount(chunkObjectCount)
    , m_alignment(alignment)
    , m_device(device)
{}

std::shared_ptr<ShaderResource::Descriptor> BufferShaderResource::fetchDescriptor()
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

size_t BufferShaderResource::allocateBuffer()
{
    m_buffers.emplace_back(m_device, bufferCreateInfo());
    m_buffers.back().allocateAndBindMemory(memoryProperties()).lock()->map();
    m_freeDescriptors.push_back(std::unordered_set<uint64_t>{});
    auto& freeSet = m_freeDescriptors.back();

    for (uint32_t val : std::views::iota(static_cast<uint32_t>(0), m_chunkObjectCount))
    {
        freeSet.insert(val);
    }

    return m_buffers.size() - 1;
}

std::shared_ptr<ShaderResource::Descriptor> BufferShaderResource::tryFetchDescriptor(
    size_t bufferId)
{
    if (auto& freeSet = m_freeDescriptors[bufferId]; !freeSet.empty())
    {
        uint64_t freeDescriptorId = freeSet.extract(freeSet.begin()).value();

        auto descriptor = ShaderResource::fetchDescriptor();

        descriptor->id.descriptorId = freeDescriptorId;
        descriptor->id.bufferId = bufferId;
        descriptor->id.resourceId = id();

        auto& buffer = m_buffers[bufferId];
        descriptor->descriptorBufferInfo =
            DescriptorBufferInfo{}
                .buffer(buffer)
                .offset(freeDescriptorId * alignment())
                .range(m_alignment);
        descriptor->memory = buffer.memory();

        return descriptor;
    }

    return nullptr;
}

void BufferShaderResource::freeDescriptor(const ShaderResource::Descriptor& descriptor)
{
    m_freeDescriptors[descriptor.id.bufferId].insert(descriptor.id.descriptorId);
}

handles::BufferCreateInfo UniformBufferShaderResource::bufferCreateInfo() const
{
    return handles::BufferCreateInfo{}
        .size(m_alignment * m_chunkObjectCount)
        .usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

VkMemoryPropertyFlags UniformBufferShaderResource::memoryProperties() const
{
    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

handles::BufferCreateInfo StorageBufferShaderResource::bufferCreateInfo() const
{
    return handles::BufferCreateInfo{}
        .size(m_alignment * m_chunkObjectCount)
        .usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

VkMemoryPropertyFlags StorageBufferShaderResource::memoryProperties() const
{
    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}


}    //  namespace vk
