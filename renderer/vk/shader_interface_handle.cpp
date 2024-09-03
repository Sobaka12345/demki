#include "shader_interface_handle.hpp"

#include <operation_context.hpp>

namespace renderer::vk {

ShaderInterfaceHandle::ShaderInterfaceHandle(ShaderResourceAllocator& uniformAllocator)
    : m_uniformAllocator(uniformAllocator)
{
    assureDescriptorCount(1);
    m_currentDescriptor = m_descriptors.begin();
}

std::shared_ptr<ShaderInterfaceHandle> ShaderInterfaceHandle::create(
    ShaderResourceAllocator& uniformAllocator)
{
    return std::shared_ptr<ShaderInterfaceHandle>{ new ShaderInterfaceHandle(uniformAllocator) };
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

size_t ShaderInterfaceHandle::descriptorCount() const
{
    return m_descriptors.size();
}

//  TO DO: UGLY
void ShaderInterfaceHandle::assureDescriptorCount(uint32_t requiredCount)
{
    while (m_descriptors.size() < requiredCount)
        m_descriptors.emplace_back(m_uniformAllocator.fetchDescriptor());
}

std::shared_ptr<ShaderResourceAllocator::Descriptor> ShaderInterfaceHandle::descriptor(size_t index)
{
    auto iter = m_descriptors.begin();
    std::advance(iter, index);
    return *iter;
}

std::shared_ptr<const ShaderResourceAllocator::Descriptor> ShaderInterfaceHandle::descriptor(
    size_t index) const
{
    auto iter = m_descriptors.begin();
    std::advance(iter, index);
    return *iter;
}

std::shared_ptr<ShaderResourceAllocator::Descriptor> ShaderInterfaceHandle::currentDescriptor()
{
    return *m_currentDescriptor;
}

std::shared_ptr<const ShaderResourceAllocator::Descriptor> ShaderInterfaceHandle::
    currentDescriptor() const
{
    return *m_currentDescriptor;
}

void ShaderInterfaceHandle::nextDescriptor()
{
    if (++m_currentDescriptor == m_descriptors.end()) m_currentDescriptor = m_descriptors.begin();
}

//  std::shared_ptr<ShaderInterfaceHandle> ShaderInterfaceHandle::create()
//  {
//      return std::shared_ptr<ShaderInterfaceHandle>{ new ShaderInterfaceHandle() };
//  }

}    //  namespace renderer::vk
