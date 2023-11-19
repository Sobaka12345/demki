#pragma once

#include "vk/ioperation_target.hpp"

#include <icompute_target.hpp>

namespace vk {

class ComputeTarget
    : virtual public IComputeTarget
    , public IOperationTarget
{};

}
