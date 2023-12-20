#pragma once

#include "handles/memory.hpp"

#include "vk/types.hpp"

#include "iresource.hpp"

#include <utils.hpp>

#include <memory>
#include <type_traits>

namespace vk {

class ShaderResource : virtual public IResource
{
public:
    struct Descriptor : public std::enable_shared_from_this<Descriptor>
    {
        struct Id
        {
            template <template <class...> typename T, typename El = ShaderResource::Descriptor::Id>
                requires std::is_iterable_v<T<El>>
            struct ContainerHasher
            {
                size_t operator()(const T<El>& e) const
                {
                    size_t result = 0;
                    uint64_t pow = 1;
                    for (ShaderResource::Descriptor::Id el : e)
                    {
                        result +=
                            el.resourceId * pow + el.descriptorId * pow * 2 + el.bufferId * pow * 3;
                        pow *= 10;
                    }

                    return result;
                }
            };

            uint64_t descriptorId = 0;
            uint64_t bufferId = 0;
            uint64_t resourceId = 0;

            bool operator<(const Id& other) const;
            bool operator==(const Id& other) const;
        } id;

        uint32_t offset() const { return dynamicOffset + descriptorBufferInfo.offset(); }

        std::weak_ptr<handles::Memory> memory;
        uint32_t dynamicOffset = 0;
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
