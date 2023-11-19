#pragma once

#include "icompute_target.hpp"

#include <memory>
#include <span>

class IShaderInterfaceHandle;

class IStorageBuffer : virtual public IComputeTarget
{
public:
    struct CreateInfo
    {
        template <typename T>
        static CreateInfo fromSpan(std::span<const T> span)
        {
            return {
                .elementLayoutSize = sizeof(T),
                .size = span.size_bytes(),
                .initialData = span.data(),
            };
        }

        uint64_t elementLayoutSize = 0;
        uint64_t size = 0;
        const void* initialData = nullptr;
    };

public:
    virtual ~IStorageBuffer() {}

    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const = 0;
    virtual void bind(::OperationContext& context) const = 0;
    virtual void draw(::OperationContext& context) const = 0;
};
