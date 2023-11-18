#include "computer.hpp"

#include "compute_target.hpp"

namespace vk {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

IComputer& Computer::addComputeTarget(IComputeTarget& target) {}

::OperationContext Computer::start(IComputeTarget& target)
{
    ::OperationContext result;
    result.emplace<vk::OperationContext>(this);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    auto& kek = get(result);


    return result;
}

void Computer::finish(::OperationContext& context)
{
    get(context).computeTarget->compute(context);
}

}    //  namespace vk
