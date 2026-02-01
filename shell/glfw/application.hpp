#pragma once

#include "../abstract/application.hpp"
#include "../abstract/vk/application.hpp"
#include "../abstract/ogl/application.hpp"

#include <glfw/window.hpp>

namespace shell::glfw {

class Application : public abstract::Application<
        shell::abstract::vk::Application,
        shell::abstract::ogl::Application
    >
{
public:
    Application(int &argc, char **argv);
    virtual ~Application();

    virtual void init() override;
    virtual int destroy(int) override;
};

}    //  namespace engine
