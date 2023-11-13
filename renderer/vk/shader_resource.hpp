#pragma once

#include "handles/buffer.hpp"

#include "vk/types.hpp"

#include "iresource.hpp"

#include <memory>

namespace vk {

class ShaderResource : virtual public IResource
{
public:
    struct Descriptor : public std::enable_shared_from_this<Descriptor>
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

        ~Descriptor();

    private:
        [[nodiscard]] static std::shared_ptr<Descriptor> create(ShaderResource& shaderResource);
        Descriptor(ShaderResource& shaderResource);

        ShaderResource& shaderResource;

        friend class ShaderResource;
    };

public:
    virtual std::shared_ptr<ShaderResource::Descriptor> fetchDescriptor();

private:
    virtual void freeDescriptor(const ShaderResource::Descriptor& descriptor);

    friend class ShaderResource::Descriptor;
};

}    //  namespace vk
