#include "qt_application.hpp"

#include <qt/opengl_window.hpp>
#include <qt/vulkan_window.hpp>

#include <QTimer>

namespace engine {

shell::IWindow* createInfoToWindow(AbstractApplication::CreateInfo createInfo)
{
    if (createInfo.gapi == GAPI::OpenGL)
    {
        return new shell::qt::OpenGLWindow{ createInfo.windowWidth, createInfo.windowHeight,
            createInfo.windowName };
    }
    else
    {
        return new shell::qt::VulkanWindow{ createInfo.windowWidth, createInfo.windowHeight,
            createInfo.windowName };
    }

    return nullptr;
}

QtApplication::QtApplication(int& argc, char** argv)
    : QGuiApplication(argc, argv)
    , AbstractApplication()
{
    init(createInfoToWindow(CreateInfo::readFromCmd(argc, argv)));
}

QtApplication::~QtApplication() {}

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

}    //  namespace engine
