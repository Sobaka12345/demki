#include "abstract_application.hpp"

#include <resources.hpp>

#include <tclap/CmdLine.h>

#include <GLFW/glfw3.h>

namespace engine {

AbstractApplication::CreateInfo AbstractApplication::CreateInfo::readFromCmd(int argc, char** argv)
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

AbstractApplication::AbstractApplication()
{
    glfwInit();
}

void AbstractApplication::init(shell::IWindow* window)
{
    m_window.reset(window);
    m_resources = std::make_unique<shell::Resources>(renderer::executablePath());
    m_graphicsContext = renderer::IGraphicsContext::create(*m_window, *m_resources);

    m_swapchain = m_graphicsContext->createSwapchain({ .framesInFlight = 2 });
}

AbstractApplication::~AbstractApplication()
{
    m_swapchain.reset();
    m_graphicsContext.reset();
    m_window.reset();

    glfwTerminate();
}

renderer::IGraphicsContext& AbstractApplication::context()
{
    return *m_graphicsContext;
}

const renderer::IGraphicsContext& AbstractApplication::context() const
{
    return *m_graphicsContext;
}

uint32_t AbstractApplication::clientWidth() const
{
    return m_swapchain->width();
}

uint32_t AbstractApplication::clientHeight() const
{
    return m_swapchain->height();
}

shell::IWindow& AbstractApplication::window()
{
    return *m_window;
}


}    //  namespace engine
