#include "uniform_provider.hpp"

namespace vk {

uint64_t UniformProvider::s_resourceCounter = 0;

UniformProvider::UniformProvider(
    const handles::Device& device, uint32_t alignment, uint32_t objectCount)
    : m_objectCount(objectCount)
    , m_alignment(alignment)
    , m_resourceId(s_resourceCounter++)
    , m_buffer(device,
          handles::BufferCreateInfo{}
              .size(alignment * objectCount)
              .usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
              .sharingMode(VK_SHARING_MODE_EXCLUSIVE))
{
    m_uniformHandles.resize(objectCount);
    m_descriptorBufferInfo =
        handles::DescriptorBufferInfo{}.buffer(m_buffer).offset(0).range(alignment);

    m_buffer.allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).lock()->map();
}

std::shared_ptr<UniformHandle> UniformProvider::tryGetUniformHandle()
{
    uint32_t index = 0;
    for (; index < m_uniformHandles.size(); ++index)
    {
        if (m_uniformHandles[index].expired())
        {
            break;
        }
    }
    if (index == m_uniformHandles.size())
    {
        return nullptr;
    }

    auto result = UniformHandle::create();
    result->resourceId = m_resourceId;
    result->offset = index * alignment();
    result->memory = m_buffer.memory();
    result->alignment = alignment();
    result->descriptorBufferInfo = m_descriptorBufferInfo;
    m_uniformHandles[index] = result;

    return result;
}
}    //  namespace vk
