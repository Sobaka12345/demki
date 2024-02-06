#include "computer.hpp"

#include <icompute_target.hpp>

namespace ogl {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

::OperationContext Computer::start(IComputeTarget& target)
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

void Computer::finish(::OperationContext& context)
{
    context.operationTarget().present(context);
}

}    //  namespace ogl
