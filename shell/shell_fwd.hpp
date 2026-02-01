#ifndef SHELL_FWD_HPP
#define SHELL_FWD_HPP

#include <gapi.hpp>

namespace shell::abstract::__private {
    GAPI_IMPL_PRIVATE_DECLARATION(Window)
    GAPI_IMPL_PRIVATE_DECLARATION(Widget)
}

namespace shell::glfw::__private {
    GAPI_IMPL_PRIVATE_DECLARATION(Window)
}

namespace shell::qt::__private {
    GAPI_IMPL_PRIVATE_DECLARATION(Window)
    GAPI_IMPL_PRIVATE_DECLARATION(Widget)
}

#endif // SHELL_FWD_HPP