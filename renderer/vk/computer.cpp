#include "computer.hpp"

namespace renderer::vk {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

renderer::OperationContext Computer::start(IComputeTarget& target)
{
    renderer::OperationContext result;
    result.emplace<vk::OperationContext>(this);

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

}    //  namespace renderer::vk
