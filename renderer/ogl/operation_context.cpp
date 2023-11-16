#include "operation_context.hpp"

#include <operation_context.hpp>

namespace ogl {

void OperationContext::submit(::OperationContext &context) {}

void OperationContext::waitForOperation(OperationContext &other) {}

void OperationContext::setScissors(Scissors scissors) const {}

void OperationContext::setViewport(Viewport viewport) const {}

}    //  namespace ogl
