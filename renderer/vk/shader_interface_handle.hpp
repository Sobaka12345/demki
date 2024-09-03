#pragma once

#include "types.hpp"
#include "buffer_shader_resource_allocator.hpp"

#include <ishader_interface_handle.hpp>

#include <list>
#include <memory>

namespace renderer::vk {

namespace handles {
class Memory;
}

class ShaderInterfaceHandle
    : public IShaderInterfaceHandle
    , public std::enable_shared_from_this<ShaderInterfaceHandle>
{
private:
    ShaderInterfaceHandle(ShaderResourceAllocator&);

public:
    //  struct TypeVisitor : public ShaderInterfaceHandleVisitor
    //  {
    //      void visit(ShaderInterfaceHandle& handle) override { this->handle = &handle; }

    //    ShaderInterfaceHandle* operator->() { return handle; }

    //  private:
    //      ShaderInterfaceHandle* handle = nullptr;
    //  };

public:
    [[nodiscard]] static std::shared_ptr<ShaderInterfaceHandle> create(
        ShaderResourceAllocator& uniformAllocator);
    ~ShaderInterfaceHandle();

    //  void reset(ShaderResource& resource);

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const override
    {
        visitor.visit(*this);
    }

    virtual void write(const void* src, size_t size) override;
    virtual const void* read(size_t size) const override;

    size_t descriptorCount() const;
    void assureDescriptorCount(uint32_t requiredCount);
    std::shared_ptr<ShaderResourceAllocator::Descriptor> descriptor(size_t index);
    std::shared_ptr<const ShaderResourceAllocator::Descriptor> descriptor(size_t index) const;
    std::shared_ptr<ShaderResourceAllocator::Descriptor> currentDescriptor();
    std::shared_ptr<const ShaderResourceAllocator::Descriptor> currentDescriptor() const;

private:
    void nextDescriptor();

private:
    ShaderResourceAllocator& m_uniformAllocator;
    std::list<std::shared_ptr<ShaderResourceAllocator::Descriptor>> m_descriptors;
    std::list<std::shared_ptr<ShaderResourceAllocator::Descriptor>>::const_iterator
        m_currentDescriptor;
};

}    //  namespace renderer::vk
