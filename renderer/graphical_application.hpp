#pragma once

#include "irender_target.hpp"
#include "igraphics_context.hpp"

#include "window.hpp"
#include "resources.hpp"

#include <memory>
#include <ratio>

class ISwapchain;
class Renderer;

class GraphicalApplication
{
public:
    struct CreateInfo
    {
        std::string windowName = "demki";
        int windowWidth = 640;
        int windowHeight = 480;
        GAPI gapi = GAPI::Vulkan;

        static CreateInfo readFromCmd(int argc, char** argv);
    };

protected:
    using TimeResolution = std::nano;

public:
    GraphicalApplication(CreateInfo createInfo);
    virtual ~GraphicalApplication();

    int exec();

    IGraphicsContext& context();
    const IGraphicsContext& context() const;
    uint32_t clientWidth() const;
    uint32_t clientHeight() const;
    Window& window();

protected:
    std::shared_ptr<ISwapchain> m_swapchain;

private:
    virtual void update(int64_t dt) = 0;
    virtual void perform() = 0;

private:
    int mainLoop();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Resources> m_resources;
    std::unique_ptr<IGraphicsContext> m_graphicsContext;
};
