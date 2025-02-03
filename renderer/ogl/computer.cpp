#include "computer.hpp"

#include <icompute_target.hpp>

namespace renderer::ogl {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

renderer::OperationContext Computer::start(IComputeTarget& target)
{
    renderer::OperationContext result;
    result.emplace<ogl::OperationContext>(this);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    return result;
}

void Computer::finish(renderer::OperationContext& context)
{
    context.operationTarget().present(context);
}

}    //  namespace renderer::ogl
