#include "shader_interface_handle.hpp"

#include "handles/memory.hpp"

#include <operation_context.hpp>

namespace vk {

ShaderInterfaceHandle::ShaderInterfaceHandle(ShaderResource& uniformAllocator)
    : m_uniformAllocator(uniformAllocator)
{
    assureDescriptorCount(1);
    m_currentDescriptor = m_descriptors.begin();
}

ShaderInterfaceHandle::~ShaderInterfaceHandle() {}

void ShaderInterfaceHandle::write(const void* src, uint32_t layoutSize)
{
    if (!currentDescriptor()->memory.expired())
    {
        nextDescriptor();
        currentDescriptor()->memory.lock()->mapped->writeAndSync(src, layoutSize,
            currentDescriptor()->offset());
    }
}

void ShaderInterfaceHandle::assureDescriptorCount(uint32_t requiredCount)
{
    while (m_descriptors.size() < requiredCount)
        m_descriptors.emplace_back(m_uniformAllocator.fetchDescriptor());
}

std::shared_ptr<ShaderResource::Descriptor> ShaderInterfaceHandle::currentDescriptor()
{
    return *m_currentDescriptor;
}

void ShaderInterfaceHandle::nextDescriptor()
{
    if (++m_currentDescriptor == m_descriptors.end()) m_currentDescriptor = m_descriptors.begin();
}

std::shared_ptr<ShaderInterfaceHandle> ShaderInterfaceHandle::create(ShaderResource& allocator)
{
    return std::shared_ptr<ShaderInterfaceHandle>{ new ShaderInterfaceHandle(allocator) };
}

}    //  namespace vk
