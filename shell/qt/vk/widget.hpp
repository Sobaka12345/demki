#ifndef SHELL_QT_VK_WIDGET_HPP
#define SHELL_QT_VK_WIDGET_HPP

#include "../../shell_fwd.hpp"

namespace shell::qt::__private {

template <typename Base>
struct Widget<Vk, Base>: Base
{
    using Base::Base;
};

}    //  namespace shell::qt

#endif // SHELL_QT_VK_WIDGET_HPP