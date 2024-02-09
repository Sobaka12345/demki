#include "shader_interface_handle.hpp"

#include "handles/memory.hpp"

#include <operation_context.hpp>

namespace renderer::vk {

ShaderInterfaceHandle::ShaderInterfaceHandle(ShaderResource& uniformAllocator)
    : m_uniformAllocator(uniformAllocator)
{
    assureDescriptorCount(1);
    m_currentDescriptor = m_descriptors.begin();
}

ShaderInterfaceHandle::~ShaderInterfaceHandle() {}

void ShaderInterfaceHandle::write(const void* src, size_t size)
{
    if (!currentDescriptor()->memory.expired())
    {
        nextDescriptor();
        currentDescriptor()->memory.lock()->mapped->writeAndSync(src, size,
            currentDescriptor()->offset());
    }
}

const void* ShaderInterfaceHandle::read(size_t size) const
{
    if (!currentDescriptor()->memory.expired())
    {
        return currentDescriptor()->memory.lock()->mapped->read(size,
            currentDescriptor()->offset());
    }

    return nullptr;
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

const std::shared_ptr<ShaderResource::Descriptor> ShaderInterfaceHandle::currentDescriptor() const
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

}    //  namespace renderer::vk
