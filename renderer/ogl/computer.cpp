#include "computer.hpp"

namespace ogl {

Computer::Computer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

::OperationContext Computer::start(IComputeTarget& target) {}

void Computer::finish(::OperationContext& target) {}

}
