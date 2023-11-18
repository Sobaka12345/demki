#pragma once

#include "vk/operation_context.hpp"
#include "ogl/operation_context.hpp"

#include <variant>

class OperationContext : public std::variant<vk::OperationContext, ogl::OperationContext>
{
public:
    using variant::variant;

    void submit()
    {
        std::visit([&](auto& context) { context.submit(*this); }, *this);
    }

    void waitForOperation(OperationContext& other)
    {
        std::visit(
            [&](auto& context) {
                context.waitForOperation(
                    std::get<typename std::remove_reference<decltype(context)>::type>(other));
            },
            *this);
    }

    IPipeline& pipeline()
    {
        IPipeline* result = nullptr;
        std::visit([&](auto& context) { result = context.pipeline(); }, *this);
        ASSERT(result, "pipeline is not bound");
        return *result;
    };

    void setScissors(Scissors scissors) const
    {
        std::visit([&](auto& context) { context.setScissors(std::move(scissors)); }, *this);
    };

    void setViewport(Viewport viewport) const
    {
        std::visit([&](auto& context) { context.setViewport(std::move(viewport)); }, *this);
    };
};

namespace vk {

inline const OperationContext& get(const ::OperationContext& context) noexcept
{
    return std::get<OperationContext>(context);
}

inline OperationContext& get(::OperationContext& context) noexcept
{
    return std::get<OperationContext>(context);
}

}

namespace ogl {

inline const OperationContext& get(const ::OperationContext& context) noexcept
{
    return std::get<OperationContext>(context);
}

inline OperationContext& get(::OperationContext& context) noexcept
{
    return std::get<OperationContext>(context);
}

}
