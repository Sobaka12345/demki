#ifndef SHELL_QT_OGL_WIDGET_HPP
#define SHELL_QT_OGL_WIDGET_HPP

#include "../../shell_fwd.hpp"

namespace shell::qt::__private {

template <typename Base>
struct Widget<Ogl, Base>: Base
{
    using Base::Base;
};

}    //  namespace shell::qt

#endif // SHELL_QT_OGL_WIDGET_HPP