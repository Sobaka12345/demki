#ifndef SHELL_QT_VK_WINDOW_HPP
#define SHELL_QT_VK_WINDOW_HPP

#include "../../shell_fwd.hpp"

#include <gapi_context.hpp>

#include <QWindow>
#include <QVulkanInstance>

namespace shell::qt::__private {

template <typename Base>
struct Window<Vk, Base>: Base
{
    template <typename ...Args>
    Window(Args... args)
        : Base(std::forward<Args>(args)...)
    {
        Base::setSurfaceType(QWindow::VulkanSurface);
    }

    [[nodiscard]] gapi::GApiContext<Vk> spawnContext() {
        gapi::GApiContext<Vk> result;
        auto qVkInstance = QWindow::vulkanInstance();
        result.instance = qVkInstance->vkInstance();

        Base::create();
        result.surface = qVkInstance->surfaceForWindow(this);
        result.iSurface = this;
        result.fetchPhysicalDevices();

        return result;
    }
};

}    //  namespace shell::qt

#endif // SHELL_QT_VK_WINDOW_HPP