#pragma once

#include "abstract_application.hpp"

#include <glfw/window.hpp>

namespace engine {

class GraphicalApplication : public AbstractApplication
{
public:
    GraphicalApplication(int& argc, char** argv);
    virtual ~GraphicalApplication();

    virtual int exec() override;
    virtual shell::glfw::Window& window() override;

private:
    int mainLoop();

private:
    std::unique_ptr<shell::glfw::Window> m_mainWindow;
};

}    //  namespace engine
