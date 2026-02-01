#pragma once

#include "../abstract/application.hpp"
#include "../abstract/vk/application.hpp"
#include "../abstract/ogl/application.hpp"

#include <QApplication>
#include <QVulkanInstance>

namespace shell::qt {

class Application
    : public QGuiApplication
    , public abstract::Application<
        shell::abstract::vk::Application,
        shell::abstract::ogl::Application
    >
{
    using Super = abstract::Application<
        shell::abstract::vk::Application,
        shell::abstract::ogl::Application
    >;

public:
    Application(int &argc, char **argv);
    virtual ~Application();

    virtual int mainLoop() override final { return exec(); }

    struct QVkInstanceBuilder : VkInstanceBuilder
    {
        QVulkanInstance& build() noexcept;
        
    private:
        using VkInstanceBuilder::build;
        using VkInstanceBuilder::VkInstanceBuilder;
        friend class Application;
    };
    virtual QVkInstanceBuilder& vkInstanceBuilder() noexcept override;

private:
    using QGuiApplication::exec;
};

}    //  namespace engine
