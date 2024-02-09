#include "graphical_application.hpp"

#include <resources.hpp>
#include <qt/vulkan_window.hpp>
#include <qt/opengl_window.hpp>
#include <glfw/vulkan_window.hpp>
#include <glfw/opengl_window.hpp>

#include <GLFW/glfw3.h>

namespace engine {

shell::IWindow* createInfoToWindow(AbstractApplication::CreateInfo createInfo)
{
    if (createInfo.gapi == GAPI::OpenGL)
    {
        return new shell::glfw::OpenGLWindow{ createInfo.windowWidth, createInfo.windowHeight,
            createInfo.windowName };
    }
    else
    {
        return new shell::glfw::VulkanWindow{ createInfo.windowWidth, createInfo.windowHeight,
            createInfo.windowName };
    }

    return nullptr;
}

GraphicalApplication::GraphicalApplication(int& argc, char** argv)
    : AbstractApplication()
{
    init(createInfoToWindow(CreateInfo::readFromCmd(argc, argv)));
}

GraphicalApplication::~GraphicalApplication() {}

int GraphicalApplication::exec()
{
    return mainLoop();
}

int GraphicalApplication::mainLoop()
{
    auto start = std::chrono::steady_clock::now();
    while (!m_window->shouldClose())
    {
        auto end = std::chrono::steady_clock::now();
        update(
            std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start)
                .count());
        start = end;
        glfwPollEvents();
        perform();
    }

    m_graphicsContext->waitIdle();

    return 0;
}

}    //  namespace engine
