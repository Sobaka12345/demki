#pragma once

#include "handles/buffer.hpp"

#include "ubo_descriptor.hpp"
#include "uniform_handle.hpp"
#include "uniform_resource.hpp"

#include <map>
#include <unordered_set>

namespace vk {

class UniformAllocator : public UniformResource
{
public:
    UniformAllocator(const handles::Device& device, uint32_t alignment, uint32_t chunkObjectCount);
    virtual std::shared_ptr<UBODescriptor> fetchUBODescriptor() override;

    uint32_t alignment() const { return m_alignment; }

private:
    size_t allocateBuffer();
    std::shared_ptr<UBODescriptor> tryFetchDescriptor(size_t bufferId);

    virtual void freeUBODescriptor(const UBODescriptor& descriptor) override;

private:
    const handles::Device& m_device;
    const uint32_t m_chunkObjectCount;
    const uint32_t m_alignment;

    handles::HandleVector<handles::Buffer> m_buffers;
    std::vector<std::unordered_set<uint64_t>> m_freeDescriptors;
};

}    //  namespace vk
