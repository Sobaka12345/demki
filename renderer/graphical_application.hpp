#pragma once

#include "irender_target.hpp"
#include "igraphics_context.hpp"

#include "window.hpp"

#include <memory>
#include <ratio>

class ISwapchain;
class Renderer;

class GraphicalApplication
{
protected:
    using TimeResolution = std::nano;

public:
    GraphicalApplication();
    ~GraphicalApplication();

    int exec();

protected:
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
    std::unique_ptr<IGraphicsContext> m_graphicsContext;
};
