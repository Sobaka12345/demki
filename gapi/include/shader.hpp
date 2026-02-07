#ifndef GAPI_SHADER_HPP
#define GAPI_SHADER_HPP

#include "../vk/shader.hpp"

namespace gapi {


template <typename T>
concept SIShader = requires(
    T& shader,
    const T& constShader) {
    { shader };
};


template <typename GApiT>
    requires SIShader<__private::Shader<GApiT>>
using Shader = __private::Shader<GApiT>;

template <typename GApiT, auto code>
consteval Shader<GApiT> parseShader() noexcept
{
    return Shader<GApiT>{__private::parseShader<code>()};
}

}

#endif // GAPI_SHADER_HPP