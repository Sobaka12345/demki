#pragma once

#include "handles/creators.hpp"

#include <iuniform_handle.hpp>

#include <memory>

namespace vk {

namespace handles {
class Memory;
}

struct UniformHandle
    : public IUniformHandle
    , public std::enable_shared_from_this<UniformHandle>
{
private:
    UniformHandle();

public:
    [[nodiscard]] static std::shared_ptr<UniformHandle> create();
    ~UniformHandle();

    virtual void accept(UniformHandleVisitor& visitor) override { visitor.visit(*this); }

    virtual void accept(UniformHandleVisitor& visitor) const override { visitor.visit(*this); }

    virtual uint32_t resourceOffset() const override;
    virtual uint64_t resource() const override;

    virtual void write(const void* src, uint32_t layoutSize) override;
    virtual void sync(uint32_t layoutSize) override;

    uint64_t resourceId = 0;
    uint32_t offset = 0;
    uint32_t alignment = 0;

    std::weak_ptr<handles::Memory> memory;
    handles::DescriptorBufferInfo descriptorBufferInfo;
    handles::DescriptorImageInfo descriptorImageInfo;
};

}    //  namespace vk
