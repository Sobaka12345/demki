#pragma once

#include "../utils.hpp"

#include <memory>
#include <concepts>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <typeindex>

namespace renderer {

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
    ShaderBlockType type = ShaderBlockType::INVALID;
    ShaderStage stage = ShaderStage::INVALID;
};

class OperationContext;
class IPipeline;
struct IPipelineBindContext;

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
    virtual ~IShaderInterfaceContainer();

    virtual void bind(renderer::OperationContext& context);
    virtual uint32_t id() const = 0;
    virtual std::type_index typeId() const = 0;
    virtual std::span<const ShaderInterfaceBinding> layout() const = 0;

    virtual std::span<const InterfaceDescriptor> uniforms() const = 0;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const = 0;

protected:
    static uint32_t createId();

private:
    static uint32_t s_id;

private:
    FragileSharedPtrMap<IPipeline*, IPipelineBindContext> m_contexts;
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

    virtual std::type_index typeId() const final override { return std::type_index{ typeid(T) }; }

    virtual uint32_t id() const final override { return sId(); };
};

template <typename T>
uint32_t SIShaderInterfaceContainer<T>::s_id = IShaderInterfaceContainer::createId();

template <typename T>
concept IsShaderInterfaceContainer = std::derived_from<T, SIShaderInterfaceContainer<T>>;

template <size_t BindingCount = 1>
using ShaderInterfaceLayout = std::array<ShaderInterfaceBinding, BindingCount>;

}    //  namespace renderer
