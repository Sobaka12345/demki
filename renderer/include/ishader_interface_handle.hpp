#pragma once

#include <ishader_interface.hpp>

#include <assert.hpp>

#include <cstdint>
#include <memory>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace renderer {

enum class ShaderInterfaceType
{
    FLOAT,
    FLOAT_X2,
    FLOAT_X3,
    FLOAT_X4,
    INT32,
    INT32_X2,
    INT32_X3,
    INT32_X4,
    UINT32,
    UINT32_X2,
    UINT32_X3,
    UINT32_X4,
    ARRAY_FLOAT,
    ARRAY_FLOAT_X2,
    ARRAY_FLOAT_X3,
    ARRAY_FLOAT_X4,
    ARRAY_INT32,
    ARRAY_INT32_X2,
    ARRAY_INT32_X3,
    ARRAY_INT32_X4,
    ARRAY_UINT32,
    ARRAY_UINT32_X2,
    ARRAY_UINT32_X3,
    ARRAY_UINT32_X4,
    MATRIX_FLOAT_2X2,
    MATRIX_FLOAT_2X3,
    MATRIX_FLOAT_2X4,
    MATRIX_FLOAT_3X2,
    MATRIX_FLOAT_3X3,
    MATRIX_FLOAT_3X4,
    MATRIX_FLOAT_4X2,
    MATRIX_FLOAT_4X3,
    MATRIX_FLOAT_4X4,
};

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
    virtual ~IShaderInterfaceHandle() {}

    virtual void accept(ShaderInterfaceHandleVisitor& visitor) = 0;
    virtual void accept(ShaderInterfaceHandleVisitor& visitor) const = 0;

    virtual void write(const void* src, size_t size) = 0;
    virtual const void* read(size_t size) const = 0;

    template <typename T>
    void write(const void* src)
    {
        write(src, sizeof(T));
    }

    template <typename T>
    const T* read() const
    {
        return static_cast<const T*>(read(sizeof(T)));
    }
};

struct IShaderResourceProvider
{
    virtual ~IShaderResourceProvider() {}

    virtual std::shared_ptr<IShaderInterfaceHandle> fetchHandle(ShaderBlockType sbt,
        uint32_t layoutSize) = 0;
};

}    //  namespace renderer
