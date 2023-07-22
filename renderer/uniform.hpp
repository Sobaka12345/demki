#pragma once

#include <memory>
#include <concepts>
#include <cstdint>
#include <span>

enum class UniformID : int32_t
{
    INVALID = -1,
    CAMERA = 0,
    POSITION = 1,
    SAMPLER = 2,
    USER,
    MAX = 100
};

enum class UniformType
{
    INVALID = -1,
    STATIC = 0,
    DYNAMIC,
    SAMPLER
};

enum class UniformStage
{
    INVALID = -1,
    VERTEX,
    FRAGMENT
};

struct UniformBinding
{
    uint32_t count = 1;
    UniformID id = UniformID::INVALID;
    UniformType type = UniformType::INVALID;
    UniformStage stage = UniformStage::INVALID;
};

class RenderContext;
class IUniformHandle;

class IUniformContainer
{
public:
    struct UniformDescriptor
    {
        std::weak_ptr<IUniformHandle> handle;
        UniformBinding binding;
    };

public:
    ~IUniformContainer(){};

    virtual void bind(::RenderContext& context) = 0;
    virtual uint32_t id() const = 0;
    virtual std::span<const UniformBinding> layout() const = 0;

    virtual std::span<const UniformDescriptor> uniforms() const = 0;
    virtual std::span<const UniformDescriptor> dynamicUniforms() const = 0;

protected:
    static uint32_t createId();

private:
    static uint32_t s_id;
};

template <typename T>
class SIUniformContainer : public IUniformContainer
{
    static uint32_t s_id;

public:
    constexpr static auto sId() { return s_id; }

    constexpr static std::span<const UniformBinding> sLayout() { return T::s_layout; }

    virtual std::span<const UniformBinding> layout() const final override { return sLayout(); };

    virtual uint32_t id() const final override { return sId(); };
};

template <typename T>
uint32_t SIUniformContainer<T>::s_id = IUniformContainer::createId();

template <typename T>
concept IsUniformContainer = std::derived_from<T, SIUniformContainer<T>>;
