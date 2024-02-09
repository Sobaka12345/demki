#include "vk/graphics_context.hpp"
#include "ogl/graphics_context.hpp"

namespace renderer {

std::unique_ptr<IGraphicsContext> IGraphicsContext::create(shell::IWindow& window,
    shell::IResources& resources)
{
    if (auto ptr = dynamic_cast<shell::IVulkanWindow*>(&window); ptr)
    {
        return std::make_unique<vk::GraphicsContext>(*ptr, resources);
    }
    else if (auto ptr = dynamic_cast<shell::IOpenGLWindow*>(&window); ptr)
    {
        return std::make_unique<ogl::GraphicsContext>(*ptr, resources);
    }

    return nullptr;
}

}    //  namespace renderer
