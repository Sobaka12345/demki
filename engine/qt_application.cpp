#include "qt_application.hpp"

#include <qt/window/vulkan_window.hpp>
#include <qt/window/opengl_window.hpp>

#include <resources.hpp>

#include <QTimer>

namespace engine {

QtApplication::QtApplication(int& argc, char** argv)
    : QGuiApplication(argc, argv)
    , AbstractApplication()
{
    m_resources = std::make_unique<shell::Resources>(renderer::executablePath());

    const auto createInfo = CreateInfo::readFromCmd(argc, argv);
    if (createInfo.gapi == GAPI::Vulkan)
    {
        m_mainWindow.reset(new shell::qt::VulkanWindow(createInfo.windowWidth,
            createInfo.windowHeight, createInfo.windowName));
    }
    else if (createInfo.gapi == GAPI::OpenGL)
    {
        m_mainWindow.reset(new shell::qt::OpenGLWindow(createInfo.windowWidth,
            createInfo.windowHeight, createInfo.windowName));
    }
}

QtApplication::~QtApplication() {}

int QtApplication::run()
{
    auto start = std::chrono::steady_clock::now();
    connect(
        m_mainWindow.get(), &shell::qt::Window::render, this,
        [start, this]() mutable {
            auto end = std::chrono::steady_clock::now();
            update(std::chrono::duration_cast<std::chrono::duration<int64_t, TimeResolution>>(
                end - start)
                       .count());
            start = end;
            perform();
        },
        Qt::DirectConnection);

    return QApplication::exec();
}

shell::qt::Window& QtApplication::window()
{
    return *m_mainWindow;
}

}    //  namespace engine
