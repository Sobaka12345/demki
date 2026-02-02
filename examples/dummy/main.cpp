#include "gapi_context.hpp"
#include <qt/application.hpp>
#include <qt/window.hpp>

#include <renderer.hpp>

#include <QVulkanInstance>

int main(int argc, char** argv)
{
    shell::qt::Application app(argc, argv);

    QVulkanInstance &qVkInstance = app.vkInstanceBuilder().build();

    shell::qt::Window<Vk> mainWindow(640, 480, "");
    mainWindow.show();
    mainWindow.setVulkanInstance(&qVkInstance);

    auto gapiContext = mainWindow.spawnContext();
    using namespace renderer;
    auto renderCtx = setup<Renderer<Vk>>(gapiContext);

    QObject::connect(&mainWindow, &shell::qt::Window<Vk>::render, [&renderCtx]() {
        Renderer<Vk>::prepareFrame(renderCtx);
        Renderer<Vk>::presentFrame(renderCtx);
    });

    QObject::connect(&mainWindow, &shell::qt::Window<Vk>::aboutToClose, [&renderCtx] () {
        teardown<Renderer<Vk>>(renderCtx);
    });

    return app.run();
}
