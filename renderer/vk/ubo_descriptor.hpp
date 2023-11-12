#pragma once

#include "handles/buffer.hpp"

#include "vk/types.hpp"

namespace vk {

class UniformResource;

struct UBODescriptor : public std::enable_shared_from_this<UBODescriptor>
{
    struct Id
    {
        uint64_t descriptorId = 0;
        uint64_t bufferId = 0;
        uint64_t resourceId = 0;

        bool operator<(const Id& other) const;
        bool operator==(const Id& other) const;
    } id;

    std::weak_ptr<handles::Memory> memory;
    DescriptorBufferInfo descriptorBufferInfo;
    DescriptorImageInfo descriptorImageInfo;

    ~UBODescriptor();

private:
    static std::shared_ptr<UBODescriptor> create(UniformResource& uniformResource);
    UBODescriptor(UniformResource& uniformResource);

    UniformResource& uniformResource;

    friend class UniformResource;
};

}    //  namespace vk
