#pragma once

#include "vk/ioperation_target.hpp"

#include <irender_target.hpp>

namespace vk {

class OperationContext;

class RenderTarget
    : virtual public IRenderTarget
    , public IOperationTarget
{
public:
};

}
