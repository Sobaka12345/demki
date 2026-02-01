#ifndef SHELL_GLFW_VK_WINDOW_HPP
#define SHELL_GLFW_VK_WINDOW_HPP

#include "../../shell_fwd.hpp"

namespace shell::glfw::__private {

template <typename Base>
struct Window<Vk, Base>: Base
{
    using Base::Base;
};

}    //  namespace shell::glfw::__private

#endif // SHELL_GLFW_VK_WINDOW_HPP