#ifndef ABSTRACT_OGL_APPLICATION_HPP
#define ABSTRACT_OGL_APPLICATION_HPP

#include <gapi_context.hpp>

namespace shell::abstract::ogl {

class Application 
{
public:
    explicit Application(int& argc, char **argv);

protected:
    void init() noexcept;
};

}

#endif // ABSTRACT_OGL_APPLICATION_HPP