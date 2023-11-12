#include "uniform_handle.hpp"

#include "handles/memory.hpp"

#include <render_context.hpp>

namespace vk {

UniformHandle::UniformHandle(UniformResource& uniformAllocator)
    : m_uniformAllocator(uniformAllocator)
{
    assureUBOCount(1);
    m_currentDescriptor = m_descriptors.begin();
}

UniformHandle::~UniformHandle() {}

void UniformHandle::write(const void* src, uint32_t layoutSize)
{
    if (!currentDescriptor()->memory.expired())
    {
        nextDescriptor();
        currentDescriptor()->memory.lock()->mapped->write(src, layoutSize,
            currentDescriptor()->descriptorBufferInfo.offset());
    }
}

void UniformHandle::assureUBOCount(uint32_t requiredCount)
{
    while (m_descriptors.size() < requiredCount)
        m_descriptors.emplace_back(m_uniformAllocator.fetchUBODescriptor());
}

std::shared_ptr<UBODescriptor> UniformHandle::currentDescriptor()
{
    return *m_currentDescriptor;
}

void UniformHandle::nextDescriptor()
{
    if (++m_currentDescriptor == m_descriptors.end()) m_currentDescriptor = m_descriptors.begin();
}

std::shared_ptr<UniformHandle> UniformHandle::create(UniformResource& allocator)
{
    return std::shared_ptr<UniformHandle>{ new UniformHandle(allocator) };
}

}    //  namespace vk
