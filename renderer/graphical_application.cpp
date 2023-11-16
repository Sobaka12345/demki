#include "graphical_application.hpp"

#include "vk/graphics_context.hpp"

#include <GLFW/glfw3.h>

GraphicalApplication::GraphicalApplication()
{
    m_window = std::make_unique<Window>(640, 480, "demki");
    m_graphicsContext = std::make_unique<vk::GraphicsContext>(*m_window);

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
