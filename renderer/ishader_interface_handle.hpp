#pragma once

#include "shader_interface.hpp"

#include <assert.hpp>

#include <cstdint>
#include <memory>

class OperationContext;

namespace ogl {
class ShaderInterfaceHandle;
}

namespace vk {
class ShaderInterfaceHandle;
}

struct ShaderInterfaceHandleVisitor
{
    virtual void visit(vk::ShaderInterfaceHandle& handle) { ASSERT(false, "not implemented"); };

    virtual void visit(ogl::ShaderInterfaceHandle& handle) { ASSERT(false, "not implemented"); };

    virtual void visit(const vk::ShaderInterfaceHandle& handle)
    {
        ASSERT(false, "not implemented");
    };

    virtual void visit(const ogl::ShaderInterfaceHandle& handle)
    {
        ASSERT(false, "not implemented");
    };
};

struct IShaderInterfaceHandle
{
    virtual void accept(ShaderInterfaceHandleVisitor& visitor) = 0;
    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const = 0;

    virtual void write(const void* src, uint32_t layoutSize) = 0;

    template <typename T>
    void write(const T* src)
    {
        write(src, sizeof(T));
    }
};

class IShaderResourceProvider
{
public:
    virtual std::shared_ptr<IShaderInterfaceHandle> fetchHandle(ShaderBlockType sbt,
        uint32_t layoutSize) = 0;

    virtual ~IShaderResourceProvider() {}
};
