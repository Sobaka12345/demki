#pragma once

#include "ipipeline.hpp"
#include "ishader_interface.hpp"
#include "ishader_resource.hpp"

#include "../utils.hpp"

#include <type_list.hpp>

#include <cstdint>
#include <span>

namespace renderer {

class IShaderInterfaceContainer
{
protected:
    using ShaderInterfaceBindings = NullType;

public:
    virtual ~IShaderInterfaceContainer();

    virtual void bind(renderer::OperationContext& context);
    virtual uint32_t id() const = 0;
    virtual std::span<const ShaderInterfaceBinding> layout() const = 0;

    auto begin() { return m_resources.begin(); }

    auto begin() const { return m_resources.begin(); }

    auto end() { return m_resources.end(); }

    auto end() const { return m_resources.end(); }

protected:
    static uint32_t createId();

protected:
    std::vector<std::shared_ptr<IShaderResource>> m_resources;

private:
    static uint32_t s_id;

private:
    std::unordered_map<IPipeline*, std::shared_ptr<IPipelineBindContext>> m_contexts;
};

template <typename T, typename... Args>
    requires std::derived_from<T, IShaderInterfaceContainer>
class ShaderInterfaceContainer : public T
{
    using ThisBindings = TypeList<Args...>;

protected:
    using ShaderInterfaceBindings =
        Append<typename T::ShaderInterfaceBindings, ThisBindings>::Result;

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

    constexpr ShaderInterfaceContainer()
    {
        IShaderInterfaceContainer::m_resources.resize(SizeOf<ShaderInterfaceBindings>::value);
    }

    const std::shared_ptr<const IShaderResource>& resource(size_t index) const
    {
        return IShaderInterfaceContainer::m_resources
            [SizeOf<typename T::ShaderInterfaceBindings>::value + index];
    }

    std::shared_ptr<IShaderResource>& resource(size_t index)
    {
        return IShaderInterfaceContainer::m_resources
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

}    //  namespace renderer
