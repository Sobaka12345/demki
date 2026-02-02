#ifndef RENDERER_FWD_HPP
#define RENDERER_FWD_HPP

#include <type_list.hpp>
#include <gapi_context.hpp>

namespace renderer {

enum class RendererType {
    DEFAULT = 0,
};

namespace __private {

template <typename GApiT, typename RendererT> [[nodiscard]] RendererT::Context setupRenderer(gapi::GApiContext<GApiT>& ctx) noexcept;
template <typename GApiT, typename RendererT> void teardownRenderer(typename RendererT::Context& ctx) noexcept;

GAPI_IMPL_PRIVATE_DECLARATION(Renderer)

}

}

#endif // RENDERER_FWD_HPP