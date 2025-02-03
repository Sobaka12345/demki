#pragma once

#include <ipool.hpp>
#include <ishader_interface_container.hpp>

namespace renderer {

struct IUniformSet {};

struct IUniformSetPool : public IPool
{
    using ElementType = IUniformSet;

    struct CreateInfo {
        size_t size = 1;
        IShaderInterfaceContainer& shaderInterfaceContainer;
    };

    virtual void bind(OperationContext& context) noexcept = 0;
};

using IUniformSetPoolBase = IPoolT<IUniformSetPool>;

}
