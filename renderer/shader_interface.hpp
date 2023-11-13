#pragma once

#include <memory>
#include <concepts>
#include <cstdint>
#include <span>

enum class InterfaceBlockID : int32_t
{
    INVALID = -1,
    IBLOCK_ID_0 = 0,
    IBLOCK_ID_1,
    IBLOCK_ID_2,
    IBLOCK_ID_3,
    IBLOCK_ID_4,
    IBLOCK_ID_5,
    IBLOCK_ID_6
};

enum class ShaderBlockType
{
    INVALID = -1,
    UNIFORM_STATIC = 0,
    UNIFORM_DYNAMIC,
    SAMPLER,
    STORAGE
};

enum class ShaderStage
{
    INVALID = -1,
    VERTEX,
    FRAGMENT,
    COMPUTE
};

struct ShaderInterfaceBinding
{
    uint32_t count = 1;
    InterfaceBlockID id = InterfaceBlockID::INVALID;
    ShaderBlockType type = ShaderBlockType::INVALID;
    ShaderStage stage = ShaderStage::INVALID;
};

class RenderContext;
class IShaderInterfaceHandle;

class IShaderInterfaceContainer
{
public:
    struct InterfaceDescriptor
    {
        std::weak_ptr<IShaderInterfaceHandle> handle;
        ShaderInterfaceBinding binding;
    };

public:
    ~IShaderInterfaceContainer(){};

    virtual void bind(::RenderContext& context) = 0;
    virtual uint32_t id() const = 0;
    virtual std::span<const ShaderInterfaceBinding> layout() const = 0;

    virtual std::span<const InterfaceDescriptor> uniforms() const = 0;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const = 0;

protected:
    static uint32_t createId();

private:
    static uint32_t s_id;
};

template <typename T>
class SIShaderInterfaceContainer : public IShaderInterfaceContainer
{
    static uint32_t s_id;

public:
    constexpr static auto sId() { return s_id; }

    constexpr static std::span<const ShaderInterfaceBinding> sLayout() { return T::s_layout; }

    virtual std::span<const ShaderInterfaceBinding> layout() const final override
    {
        return sLayout();
    };

    virtual uint32_t id() const final override { return sId(); };
};

template <typename T>
uint32_t SIShaderInterfaceContainer<T>::s_id = IShaderInterfaceContainer::createId();

template <typename T>
concept IsShaderInterfaceContainer = std::derived_from<T, SIShaderInterfaceContainer<T>>;

template <size_t BindingCount = 1>
using ShaderInterfaceLayout = std::array<ShaderInterfaceBinding, BindingCount>;
