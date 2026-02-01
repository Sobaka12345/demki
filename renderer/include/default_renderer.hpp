#ifndef RENDERER_SWAPCHAIN_HPP
#define RENDERER_DEFAULTRENDERER_HPP

//#include "../ogl/default_renderer.hpp"
#include "../vk/default_renderer.hpp"

namespace renderer 
{

template <typename T>
concept SIDefaultRenderer = requires(
    T& defaultrenderer,
    const T& constBuffer) {
    { defaultrenderer };
};

template <typename GApiT>
    requires SIDefaultRenderer<__private::DefaultRenderer<GApiT>>
struct DefaultRenderer : __private::DefaultRenderer<GApiT>
{};

}


#endif // RENDERER_DEFAULT_RENDERER_HPP