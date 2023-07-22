#pragma once

#include <assert.hpp>

#include <cstdint>
#include <memory>

class RenderContext;

namespace ogl {
class UniformHandle;
}

namespace vk {
class UniformHandle;
}

struct UniformHandleVisitor
{
    virtual void visit(vk::UniformHandle& handle) { ASSERT(false, "not implemented"); };

    virtual void visit(ogl::UniformHandle& handle) { ASSERT(false, "not implemented"); };

    virtual void visit(const vk::UniformHandle& handle) { ASSERT(false, "not implemented"); };

    virtual void visit(const ogl::UniformHandle& handle) { ASSERT(false, "not implemented"); };
};

struct IUniformHandle
{
    virtual void accept(UniformHandleVisitor& visitor) = 0;
    virtual void accept(UniformHandleVisitor& visitor) const = 0;

    virtual uint64_t resource() const = 0;
    virtual uint32_t resourceOffset() const = 0;

    virtual void write(const void* src, uint32_t layoutSize) = 0;
    virtual void sync(uint32_t layoutSize) = 0;

    template <typename T>
    void write(const T* src)
    {
        write(src, sizeof(T));
    }

    template <typename T>
    void sync()
    {
        sync(sizeof(T));
    }
};

class IUniformProvider
{
public:
    virtual std::shared_ptr<IUniformHandle> uniformHandle(uint32_t layoutSize) = 0;

    virtual ~IUniformProvider() {}
};
