#include "qt_application.hpp"

#include <qt/opengl_window.hpp>
#include <qt/vulkan_window.hpp>

#include <resources.hpp>

#include <QTimer>

namespace engine {

QtApplication::QtApplication(int& argc, char** argv)
    : QGuiApplication(argc, argv)
    , AbstractApplication()
{
    m_resources = std::make_unique<shell::Resources>(renderer::executablePath());

    const auto createInfo = CreateInfo::readFromCmd(argc, argv);
    if (createInfo.gapi == GAPI::OpenGL)
    {
        m_mainWindow.reset(new shell::qt::OpenGLWindow{
            createInfo.windowWidth, createInfo.windowHeight, createInfo.windowName });
    }
    else
    {
        m_mainWindow.reset(new shell::qt::VulkanWindow{
            createInfo.windowWidth, createInfo.windowHeight, createInfo.windowName });
    }

    m_graphicsContext = renderer::IGraphicsContext::create(*m_mainWindow, *m_resources);

    m_swapchain = m_graphicsContext->createSwapchain({ .framesInFlight = 2 });
}

QtApplication::~QtApplication()
{
    m_swapchain.reset();
    m_graphicsContext.reset();
    m_mainWindow.reset();
}

int QtApplication::exec()
{
    QTimer* timer = new QTimer(this);
    timer->setInterval(0);
    timer->setSingleShot(false);
    auto start = std::chrono::steady_clock::now();
    connect(timer, &QTimer::timeout, this, [start, this]() mutable {
        auto end = std::chrono::steady_clock::now();
        update(
            std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(end - start)
                .count());
        start = end;
        perform();
    });
    timer->start();

    return QApplication::exec();
}

shell::qt::Window& QtApplication::window()
{
    return *m_mainWindow;
}

}    //  namespace engine
