#ifndef SHELL_QT_OGL_WINDOW_HPP
#define SHELL_QT_OGL_WINDOW_HPP

#include "../../shell_fwd.hpp"

namespace shell::qt::__private {

template <typename Base>
struct Window<Ogl, Base>: Base
{
    using Base::Base;
};

}    //  namespace shell::qt

#endif // SHELL_QT_OGL_WINDOW_HPP