#include "graphical_application.hpp"

#include "vk/graphics_context.hpp"
#include "ogl/graphics_context.hpp"

#include <GLFW/glfw3.h>
#include <utils.hpp>

#include <tclap/CmdLine.h>

GraphicalApplication::CreateInfo GraphicalApplication::CreateInfo::readFromCmd(int argc,
    char** argv)
{
    CreateInfo result;

    try
    {
        TCLAP::CmdLine cmd("Graphical application", ' ');
        TCLAP::ValueArg<std::string> gapiArg(
            "g", "gapi", "Graphics API to use", false, "opengl, vulkan", "string");

        cmd.add(gapiArg);

        cmd.parse(argc, argv);
        if (gapiArg.isSet())
        {
            if (auto value = gapiArg.getValue(); value == "opengl")
            {
                result.gapi = GAPI::OpenGL;
            }
            else if (value == "vulkan")
            {
                result.gapi = GAPI::Vulkan;
            }
            else
            {
                throw TCLAP::ArgException("invalid argument exception",
                    "undefined",
                    "invalid argument value, possible values are: opengl, vulkan");
            }
        }
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return result;
}

GraphicalApplication::GraphicalApplication(CreateInfo createInfo)
{
    m_window = std::make_unique<Window>(
        createInfo.windowWidth, createInfo.windowHeight, createInfo.windowName);
    m_resources = std::make_unique<Resources>(executablePath());


    switch (createInfo.gapi)
    {
        case GAPI::OpenGL:
            m_graphicsContext = std::make_unique<ogl::GraphicsContext>(*m_window, *m_resources);
            break;
        case GAPI::Vulkan:
        default: m_graphicsContext = std::make_unique<vk::GraphicsContext>(*m_window, *m_resources);
    }

    m_swapchain = m_graphicsContext->createSwapchain({ .framesInFlight = 2 });
}

GraphicalApplication::~GraphicalApplication()
{
    m_swapchain.reset();
    m_graphicsContext.reset();
    m_window.reset();
}

int GraphicalApplication::exec()
{
    return mainLoop();
}

IGraphicsContext& GraphicalApplication::context()
{
    return *m_graphicsContext;
}

const IGraphicsContext& GraphicalApplication::context() const
{
    return *m_graphicsContext;
}

uint32_t GraphicalApplication::clientWidth() const
{
    return m_swapchain->width();
}

uint32_t GraphicalApplication::clientHeight() const
{
    return m_swapchain->height();
}

Window& GraphicalApplication::window()
{
    return *m_window;
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
