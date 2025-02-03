#pragma once

#include "../utils.hpp"

#include "ibuffer.hpp"
#include "icompute_target.hpp"
#include "ishader_resource.hpp"

#include <span>

#include <boost/pfr.hpp>

namespace renderer {

class IShaderInterfaceHandle;

class IStorageBuffer
    : public IComputeTarget
    , public IBuffer
    , public IShaderResource
{
public:
    struct CreateInfo : public IBuffer::CreateInfo
    {
        template <typename T>
        CreateInfo(std::span<const T> data, bool normalized = false)
            : normalized(normalized)
            , dataTypeMetaInfo(StructMetaInfo::fromType<T>())
        {}

        bool normalized;
        const void* initialData;
        StructMetaInfo dataTypeMetaInfo;
    };

public:
    virtual ~IStorageBuffer() {}

    virtual void draw(OperationContext& context) const = 0;
};

}    //  namespace renderer
