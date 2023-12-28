#include "graphical_application.hpp"

#include "vk/graphics_context.hpp"

#include <GLFW/glfw3.h>
#include <utils.hpp>

GraphicalApplication::GraphicalApplication(
    std::string name, uint32_t windowWidth, uint32_t windowHeight)
{
    m_window = std::make_unique<Window>(windowWidth, windowHeight, name);
    m_resources = std::make_unique<Resources>(executablePath());
    m_graphicsContext = std::make_unique<vk::GraphicsContext>(*m_window, *m_resources);

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
