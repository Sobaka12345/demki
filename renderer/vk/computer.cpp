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
    m_targets.emplace(std::pair<size_t, IComputeTarget&>{ result.id(), target });

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    return result;
}

void Computer::finish(::OperationContext& context)
{
    auto iter = m_targets.find(context.id());
    iter->second.compute(context);
    m_targets.erase(iter);
}

}    //  namespace vk
