#ifndef GAPI_SWAPCHAIN_HPP
#define GAPI_SWAPCHAIN_HPP

#include "gapi.hpp"
#include "../vk/swapchain.hpp"

namespace gapi 
{

template <typename T>
concept SISwapchain = requires(
    T& swapchain,
    const T& constBuffer) {
    { swapchain.handle };
};

template <typename GApiT>
    requires SISwapchain<__private::Swapchain<GApiT>>
struct Swapchain : __private::Swapchain<GApiT>
{};

}


#endif // GAPI_SWAPCHAIN_HPP