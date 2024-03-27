#include "graphical_application.hpp"

#include <resources.hpp>
#include <qt/vulkan_window.hpp>
#include <qt/opengl_window.hpp>
#include <glfw/vulkan_window.hpp>
#include <glfw/opengl_window.hpp>

#include <GLFW/glfw3.h>

namespace engine {

GraphicalApplication::GraphicalApplication(int& argc, char** argv)
    : AbstractApplication()
{
    m_resources = std::make_unique<shell::Resources>(renderer::executablePath());

    const auto createInfo = CreateInfo::readFromCmd(argc, argv);
    if (createInfo.gapi == GAPI::Vulkan)
    {
        m_mainWindow.reset(new shell::glfw::VulkanWindow(createInfo.windowWidth,
            createInfo.windowHeight, createInfo.windowName));
    }
    else if (createInfo.gapi == GAPI::OpenGL)
    {
        m_mainWindow.reset(new shell::glfw::OpenGLWindow(createInfo.windowWidth,
            createInfo.windowHeight, createInfo.windowName));
    }
}

GraphicalApplication::~GraphicalApplication()
{
    m_mainWindow.reset();
}

int GraphicalApplication::exec()
{
    return mainLoop();
}

shell::glfw::Window& GraphicalApplication::window()
{
    return *m_mainWindow;
}

int GraphicalApplication::mainLoop()
{
    auto start = std::chrono::steady_clock::now();
    while (!m_mainWindow->shouldClose())
    {
        auto end = std::chrono::steady_clock::now();
        update(
            std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start)
                .count());
        start = end;
        glfwPollEvents();
        perform();
    }

    context().waitIdle();

    return 0;
}

}    //  namespace engine
