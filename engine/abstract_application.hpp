#pragma once

#include <iresources.hpp>
#include <irender_target.hpp>
#include <igraphics_context.hpp>

#include <memory>
#include <ratio>

namespace engine {

class AbstractApplication
{
protected:
    using TimeResolution = std::nano;

public:
    struct CreateInfo
    {
        std::string windowName = "demki";
        int windowWidth = 640;
        int windowHeight = 480;
        GAPI gapi = GAPI::Vulkan;

        static CreateInfo readFromCmd(int argc, char** argv);
    };

public:
    AbstractApplication();
    virtual ~AbstractApplication();

    uint32_t clientWidth() const;
    uint32_t clientHeight() const;
    shell::IWindow& window();

    renderer::IGraphicsContext& context();
    const renderer::IGraphicsContext& context() const;

    virtual int exec() = 0;

protected:
    void init(shell::IWindow* window);

    virtual void update(int64_t dt) = 0;
    virtual void perform() = 0;

protected:
    std::shared_ptr<renderer::ISwapchain> m_swapchain;

    std::unique_ptr<shell::IWindow> m_window;
    std::unique_ptr<shell::IResources> m_resources;
    std::unique_ptr<renderer::IGraphicsContext> m_graphicsContext;
};

}    //  namespace engine
