#ifndef GAPI_HPP
#define GAPI_HPP

#define GAPI_IMPL_PRIVATE_DECLARATION(typeName)  \
template <typename GApiT, typename...TT> \
struct typeName;

#include <type_list.hpp>

struct Vk {};
struct Ogl {};

using GApiTypeList = TypeList<Vk, Ogl>;

#endif // GAPI_HPP