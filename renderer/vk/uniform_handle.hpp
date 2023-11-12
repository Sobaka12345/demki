#pragma once

#include "vk/types.hpp"
#include "vk/uniform_allocator.hpp"

#include <iuniform_handle.hpp>

#include <list>
#include <memory>

namespace vk {

class UBODescriptor;
class UniformResource;

namespace handles {
class Memory;
}

class UniformHandle
    : public IUniformHandle
    , public std::enable_shared_from_this<UniformHandle>
{
private:
    UniformHandle(UniformResource&);

public:
    [[nodiscard]] static std::shared_ptr<UniformHandle> create(UniformResource&);
    ~UniformHandle();

    virtual void accept(UniformHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(UniformHandleVisitor& visitor) const override { visitor.visit(*this); }

    virtual void write(const void* src, uint32_t layoutSize) override;

    void assureUBOCount(uint32_t requiredCount);
    std::shared_ptr<UBODescriptor> currentDescriptor();

private:
    void nextDescriptor();

private:
    UniformResource& m_uniformAllocator;
    std::list<std::shared_ptr<UBODescriptor>> m_descriptors;
    std::list<std::shared_ptr<UBODescriptor>>::const_iterator m_currentDescriptor;
};

}    //  namespace vk
