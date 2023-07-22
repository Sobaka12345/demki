#pragma once

#include "assert.hpp"
#include "render_context.hpp"

#include <cstdint>


class IRenderTarget;

namespace vk {
class Swapchain;
}

namespace ogl {
class Swapchain;
}

class RenderInfoVisitor
{
public:
    virtual void populateRenderInfo(const vk::Swapchain& swapchain)
    {
        ASSERT(false, "not implemented");
    };

    virtual void populateRenderInfo(const ogl::Swapchain& swapchain)
    {
        ASSERT(false, "not implemented");
    };
};

class IRenderer
{
public:
    struct CreateInfo
    {
        uint32_t samplesCount = 1;
    };

public:
    virtual IRenderer& addRenderTarget(IRenderTarget& target) = 0;
    virtual RenderContext start(IRenderTarget& target) = 0;

    virtual ~IRenderer(){};
};
