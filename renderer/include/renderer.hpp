#ifndef RENDERER_SWAPCHAIN_HPP
#define RENDERER_DEFAULTRENDERER_HPP

//#include "../ogl/default_renderer.hpp"
#include "../vk/renderer.hpp"
#include "../renderer_fwd.hpp"

namespace renderer 
{

template <typename T>
concept SIRenderer = requires(
    T& renderer,
    const T& constrenderer) {
    { renderer };
};

template <typename GApiT, RendererType type = RendererType::DEFAULT>
    requires SIRenderer<__private::Renderer<GApiT, std::integral_constant<RendererType, type>>>
struct Renderer : __private::Renderer<GApiT, std::integral_constant<RendererType, type>>
{
    using Impl = __private::Renderer<GApiT, std::integral_constant<RendererType, type>>;
    using GApi = GApiT;
};

template <SIRenderer RendererT> 
[[nodiscard]] RendererT::Context setup(gapi::GApiContext<typename RendererT::GApi>& ctx) noexcept
{
    return __private::setupRenderer<typename RendererT::GApi, typename RendererT::Impl>(ctx);
}

template <SIRenderer RendererT> 
void teardown(typename RendererT::Context& ctx) noexcept
{
    __private::teardownRenderer<typename RendererT::GApi, typename RendererT::Impl>(ctx);
}

}


#endif // RENDERER_DEFAULT_RENDERER_HPP