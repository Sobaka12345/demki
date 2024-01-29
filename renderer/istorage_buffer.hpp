#pragma once

#include "icompute_target.hpp"

#include <type_utils.hpp>

#include <memory>
#include <span>

#include <boost/pfr.hpp>

class IShaderInterfaceHandle;

class IStorageBuffer : virtual public IComputeTarget
{
public:
    struct CreateInfo
    {
        template <typename T>
        CreateInfo(std::span<const T> data, bool normalized = false)
            : normalized(normalized)
            , initialData(data.data())
            , initialDataSize(data.size())
            , dataTypeMetaInfo(StructMetaInfo::fromType<T>())
        {}

        bool normalized;
        const void* initialData;
        size_t initialDataSize;
        StructMetaInfo dataTypeMetaInfo;
    };

public:
    virtual ~IStorageBuffer() {}

    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const = 0;
    virtual void bind(::OperationContext& context) const = 0;
    virtual void draw(::OperationContext& context) const = 0;
};
