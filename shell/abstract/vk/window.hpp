#ifndef SHELL_ABSTRACT_VK_WINDOW
#define SHELL_ABSTRACT_VK_WINDOW

#include "../../shell_fwd.hpp"

#include <surface.hpp>

namespace shell::abstract::__private {

template <>
struct Window<Vk> : gapi::Surface<Vk>
{};

}

#endif // SHELL_ABSTRACT_VK_WINDOW