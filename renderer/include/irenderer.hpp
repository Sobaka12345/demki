#pragma once

#include "../operation_context.hpp"

#include <assert.hpp>

#include <cstdint>

namespace renderer {

namespace vk {
class Swapchain;
}

namespace ogl {
class Swapchain;
}

class IRenderTarget;

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
        Multisampling multisampling = Multisampling::MSA_1X;
        glm::vec4 clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

public:
    virtual OperationContext start(IRenderTarget& target) = 0;
    virtual void finish(OperationContext& context) = 0;

    virtual ~IRenderer(){};
};

}    //  namespace renderer
