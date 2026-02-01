#ifndef SHELL_GLFW_OGL_WINDOW_HPP
#define SHELL_GLFW_OGL_WINDOW_HPP

#include "../../shell_fwd.hpp"

namespace shell::glfw::__private {

template <typename Base>
struct Window<Ogl, Base>: Base
{
    using Base::Base;
};

}    //  namespace shell::glfw

#endif // SHELL_GLFW_OGL_WINDOW_HPP