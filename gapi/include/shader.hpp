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


template <typename GApiT, auto ... Args>
    requires SIShader<__private::Shader<GApiT, Args...>>
using Shader = __private::Shader<GApiT, Args...>;

}

#endif // GAPI_SHADER_HPP