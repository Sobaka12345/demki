#ifndef RENDERER_HPP
#define RENDERER_HPP

//#include "ogl/default_renderer.hpp"
#include "vk/default_renderer.hpp"

namespace renderer {

    template <typename GApiT, typename ImplT>
    concept IRenderer = requires(
        ImplT& renderer,
        const ImplT& constRenderer,
        gapi::GApiContext<GApiT>& gApiContext,
        typename ImplT::Context& renderContext) {
        { renderer.init(gApiContext) } -> std::same_as<typename ImplT::Context>;
        { prepareSwapchain(renderContext) } -> std::same_as<void>;
        { presentSwapchain(renderContext) } -> std::same_as<void>;
        { render(renderContext) } -> std::same_as<void>;

    };

    template <typename GApiT, template <typename > typename RendererImplT>
        requires IRenderer<GApiT, RendererImplT<GApiT>>
    struct Renderer : RendererImplT<GApiT> {};
}

#endif // RENDERER_HPP