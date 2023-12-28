#include "renderer.hpp"

#include <irender_target.hpp>

namespace ogl {

Renderer::Renderer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

::OperationContext Renderer::start(::IRenderTarget& target)
{
    ::OperationContext result;
    result.emplace<ogl::OperationContext>(this);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    return result;
}

void Renderer::finish(::OperationContext& context) {}

}    //  namespace ogl
