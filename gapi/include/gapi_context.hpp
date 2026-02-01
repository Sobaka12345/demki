#ifndef GAPI_CONTEXT
#define GAPI_CONTEXT

#include "../vk/gapi_context.hpp"
#include "../ogl/gapi_context.hpp"

namespace gapi 
{

struct ISurface;

template <typename T>
concept SIGApiContext = requires(
    T& context,
    const T& constBuffer) {
        {context}; //TO DEFINE
};

template<typename GApiT>
    requires SIGApiContext<__private::GApiContext<GApiT>>
struct GApiContext : __private::GApiContext<GApiT>
{
    // TO DO: should I merge ISurface with 
    // Surface<> if that leads to diamond virtual inheritance?
    ISurface* iSurface = nullptr;
};

}

#endif // GAPI_CONTEXT
