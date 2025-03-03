#pragma once

#include "../operation_context.hpp"

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
        CREATE_INFO_PROPERTY(Multisampling, multisampling, Multisampling::MSA_1X)
        CREATE_INFO_PROPERTY(glm::vec4, clearValue, {})
    };

public:
    virtual OperationContext start(IRenderTarget& target) = 0;
    virtual void finish(OperationContext& context) = 0;

    virtual ~IRenderer(){};
};

}    //  namespace renderer
