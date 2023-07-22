#pragma once

#include "vk/render_context.hpp"
#include "ogl/render_context.hpp"

#include <variant>

class RenderContext : public std::variant<vk::RenderContext, ogl::RenderContext>
{
public:
    using variant::variant;

    IPipeline& pipeline()
    {
        IPipeline* result = nullptr;
        std::visit([&](auto& context) { result = context.pipeline; }, *this);
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

inline const RenderContext& get(const ::RenderContext& context) noexcept
{
    return std::get<RenderContext>(context);
}

inline RenderContext& get(::RenderContext& context) noexcept
{
    return std::get<RenderContext>(context);
}

}

namespace ogl {

inline const RenderContext& get(const ::RenderContext& context) noexcept
{
    return std::get<RenderContext>(context);
}

inline RenderContext& get(::RenderContext& context) noexcept
{
    return std::get<RenderContext>(context);
}

}
