#pragma once

#include "handles/buffer.hpp"

#include "uniform_handle.hpp"

#include <map>

namespace vk {

class UniformProvider
{
    static uint64_t s_resourceCounter;

public:
    UniformProvider(const handles::Device& device, uint32_t alignment, uint32_t objectCount);
    std::shared_ptr<UniformHandle> tryGetUniformHandle();

    uint32_t alignment() const { return m_alignment; }

    uint32_t objectCount() const { return m_objectCount; }

private:
    const uint64_t m_resourceId;
    const uint32_t m_objectCount;
    const uint32_t m_alignment;
    DescriptorBufferInfo m_descriptorBufferInfo;

    handles::Buffer m_buffer;
    std::vector<std::weak_ptr<UniformHandle>> m_uniformHandles;
};

}    //  namespace vk
