#ifndef SHELL_ABSTRACT_OGL_WINDOW
#define SHELL_ABSTRACT_OGL_WINDOW

#include "../../shell_fwd.hpp"

#include <surface.hpp>

namespace shell::abstract::__private {

template <>
struct Window<Ogl> : gapi::Surface<Ogl>
{};

}

#endif // SHELL_ABSTRACT_OGL_WINDOW