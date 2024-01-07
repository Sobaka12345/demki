#include "computer.hpp"

namespace vk {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

::OperationContext Computer::start(IComputeTarget& target)
{
    ::OperationContext result;
    result.emplace<vk::OperationContext>(this);

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

}    //  namespace vk
