#pragma once

#include "types.hpp"
#include "buffer_shader_resource.hpp"
#include "shader_resource.hpp"

#include <ishader_interface_handle.hpp>

#include <list>
#include <memory>

namespace vk {

namespace handles {
class Memory;
}

class ShaderInterfaceHandle
    : public IShaderInterfaceHandle
    , public std::enable_shared_from_this<ShaderInterfaceHandle>
{
private:
    ShaderInterfaceHandle(ShaderResource&);

public:
    struct TypeVisitor : public ShaderInterfaceHandleVisitor
    {
        void visit(ShaderInterfaceHandle& handle) override { this->handle = &handle; }

        ShaderInterfaceHandle* operator->() { return handle; }

    private:
        ShaderInterfaceHandle* handle = nullptr;
    };

public:
    [[nodiscard]] static std::shared_ptr<ShaderInterfaceHandle> create(ShaderResource&);
    ~ShaderInterfaceHandle();

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const override
    {
        visitor.visit(*this);
    }

    virtual void write(const void* src, size_t size) override;
    virtual const void* read(size_t size) const override;

    void assureDescriptorCount(uint32_t requiredCount);
    std::shared_ptr<ShaderResource::Descriptor> currentDescriptor();
    const std::shared_ptr<ShaderResource::Descriptor> currentDescriptor() const;

private:
    void nextDescriptor();

private:
    ShaderResource& m_uniformAllocator;
    std::list<std::shared_ptr<ShaderResource::Descriptor>> m_descriptors;
    std::list<std::shared_ptr<ShaderResource::Descriptor>>::const_iterator m_currentDescriptor;
};

}    //  namespace vk
