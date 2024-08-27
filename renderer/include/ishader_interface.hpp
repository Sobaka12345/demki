#pragma once

#include "ishader_resource.hpp"

#include "../utils.hpp"

#include <type_list.hpp>

#include <memory>
#include <cstdint>
#include <span>

namespace renderer {

enum class ShaderBlockType : int16_t
{
    INVALID = -1,
    UNIFORM_STATIC = 0,
    UNIFORM_DYNAMIC,
    SAMPLER,
    STORAGE
};

enum class ShaderStage : int16_t
{
    INVALID = -1,
    VERTEX,
    FRAGMENT,
    COMPUTE
};

struct ShaderInterfaceBinding
{
    int16_t count = 1;
    ShaderBlockType type = ShaderBlockType::INVALID;
    ShaderStage stage = ShaderStage::INVALID;
};

template <typename BindingTypeT,
    ShaderBlockType blockTypeArg,
    ShaderStage stageArg,
    int16_t countArg = 1>
struct ShaderInterfaceBindingMeta
{
    using BindingType = BindingTypeT;

    enum : int16_t
    {
        type = static_cast<int16_t>(blockTypeArg),
        stage = static_cast<int16_t>(stageArg),
        count = static_cast<int16_t>(countArg)
    };
};

class OperationContext;
class IPipeline;
struct IPipelineBindContext;

class IShaderInterfaceContainer
{
protected:
    using ShaderInterfaceBindings = NullType;

public:
    struct InterfaceDescriptor
    {
        std::shared_ptr<IShaderResource> resource;
        ShaderInterfaceBinding binding;
    };

public:
    virtual ~IShaderInterfaceContainer();

    virtual void bind(renderer::OperationContext& context);
    virtual uint32_t id() const = 0;
    virtual std::span<const ShaderInterfaceBinding> layout() const = 0;

    virtual std::span<const InterfaceDescriptor> uniforms() const = 0;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const = 0;

protected:
    static uint32_t createId();


protected:
    std::vector<IShaderInterfaceContainer::InterfaceDescriptor> m_descriptors;

private:
    static uint32_t s_id;

private:
    FragileSharedPtrMap<IPipeline*, IPipelineBindContext> m_contexts;
};

template <typename T, typename... Args>
    requires std::derived_from<T, IShaderInterfaceContainer>
class ShaderInterfaceContainer : public T
{
protected:
    using ShaderInterfaceBindings =
        Append<typename T::ShaderInterfaceBindings, TypeList<Args...>>::Result;

    template <typename... Bindings>
    struct LayoutCreator
    {
        static constexpr auto create()
        {
            return std::array<ShaderInterfaceBinding, SizeOf<ShaderInterfaceBindings>::value>{
                (ShaderInterfaceBinding{
                    static_cast<int16_t>(Bindings::count),
                    static_cast<ShaderBlockType>(Bindings::type),
                    static_cast<ShaderStage>(Bindings::stage),
                })...,
            };
        }
    };

    explicit ShaderInterfaceContainer()
    {
        IShaderInterfaceContainer::m_descriptors.insert(
            IShaderInterfaceContainer::m_descriptors.end(),
            { IShaderInterfaceContainer::InterfaceDescriptor{
                .binding = ShaderInterfaceBinding{
                    .count = static_cast<int16_t>(Args::count),
                    .type = static_cast<ShaderBlockType>(Args::type),
                    .stage = static_cast<ShaderStage>(Args::stage),
                } }... });
    }

    const IShaderInterfaceContainer::InterfaceDescriptor& descriptor(size_t index) const
    {
        return IShaderInterfaceContainer::m_descriptors
            [SizeOf<typename T::ShaderInterfaceBindings>::value + index];
    }

    IShaderInterfaceContainer::InterfaceDescriptor& descriptor(size_t index)
    {
        return IShaderInterfaceContainer::m_descriptors
            [SizeOf<typename T::ShaderInterfaceBindings>::value + index];
    }

public:
    static uint32_t sId()
    {
        static uint32_t s_id = T::createId();
        return s_id;
    }

    static constexpr auto s_layout =
        Apply<LayoutCreator, ShaderInterfaceBindings>::Result::create();

    virtual uint32_t id() const override { return sId(); }

    virtual std::span<const ShaderInterfaceBinding> layout() const override { return s_layout; }
};

template <size_t BindingCount = 1>
using ShaderInterfaceLayout = std::array<ShaderInterfaceBinding, BindingCount>;

}    //  namespace renderer
