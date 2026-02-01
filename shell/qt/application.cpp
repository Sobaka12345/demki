#include "application.hpp"

#include "window.hpp"
#include <QVulkanInstance>

namespace shell::qt {

Application::Application(int &argc, char **argv)
    : QGuiApplication(argc, argv)
    , abstract::Application<
        shell::abstract::vk::Application,
        shell::abstract::ogl::Application
    >(argc, argv)
{}

Application::~Application() {}

Application::QVkInstanceBuilder& Application::vkInstanceBuilder() noexcept
{
    static QVkInstanceBuilder s_result(*this);
    return s_result;
}

QVulkanInstance& Application::QVkInstanceBuilder::build() noexcept
{
    static QVulkanInstance& s_result = [this]() -> QVulkanInstance& {
        static QVulkanInstance s_instance;
        
        QVulkanInstance tmp;
        tmp.create();
        auto extensions = tmp.extensions();
        tmp.destroy();
        
        QList<const char*> cstrs;
        cstrs.reserve(extensions.size());
        for (const QByteArray& ba : extensions) {
            cstrs.push_back(ba.constData());
        }

        enabledExtensionCount = cstrs.size();
        ppEnabledExtensionNames = cstrs.data();

        s_instance.setVkInstance(VkInstanceBuilder::build());
        ASSERT(s_instance.create(), "failed to create qt vulkan instance");
        return s_instance;
    }();
    return s_result;
}

}    //  namespace engine
