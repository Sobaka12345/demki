#include "gapi_context.hpp"
#include <qt/application.hpp>
#include <qt/window.hpp>

#include <default_renderer.hpp>

#include <QVulkanInstance>

int main(int argc, char** argv)
{
    shell::qt::Application app(argc, argv);

    QVulkanInstance &qVkInstance = app.vkInstanceBuilder().build();

    shell::qt::Window<Vk> mainWindow(640, 480, "");
    mainWindow.show();
    mainWindow.setVulkanInstance(&qVkInstance);

    auto gapiContext = mainWindow.spawnContext();
    auto renderCtx = renderer::DefaultRenderer<Vk>::init(gapiContext);

    QObject::connect(&mainWindow, &shell::qt::Window<Vk>::render, [&renderCtx]() {
        renderer::DefaultRenderer<Vk>::render(renderCtx);
    });

    QObject::connect(&mainWindow, &shell::qt::Window<Vk>::aboutToClose, [&renderCtx] () {
        renderer::DefaultRenderer<Vk>::teardown(renderCtx);
    });

    return app.run();
}
