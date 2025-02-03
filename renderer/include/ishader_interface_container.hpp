#pragma once

#include "ipipeline.hpp"
#include "ishader_interface.hpp"

#include <type_list.hpp>

#include <cstdint>

namespace renderer {

class IUniformSetPool;
class IGraphicsContext;

class IShaderInterfaceContainer
{
protected:
    using ShaderInterfaceBindings = NullType;

public:
    IShaderInterfaceContainer(IGraphicsContext& context)
        : m_context(context)
    {}
    virtual ~IShaderInterfaceContainer() {};

    virtual IUniformSetPool* createPool(size_t size) = 0;
    virtual uint32_t id() const = 0;
    virtual std::span<const ShaderInterfaceBinding> layout() const = 0;

protected:
    static uint32_t createId();

private:
    static uint32_t s_id;
    IGraphicsContext& m_context;
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
                    static_cast<int16_t>(Bindings::size),
                    static_cast<ShaderBlockType>(Bindings::type),
                    static_cast<ShaderStage>(Bindings::stage),
                })...,
            };
        }
    };

public:
    virtual uint32_t id() const override
    {
        static uint32_t s_id = T::createId();
        return s_id;
    }

    virtual std::span<const ShaderInterfaceBinding> layout() const override {
        return s_layout;
    }

protected:
    static constexpr auto s_layout = Apply<LayoutCreator, ShaderInterfaceBindings>::Result::create();
};

}    //  namespace renderer
