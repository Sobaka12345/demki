#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

template <typename T>
struct ShaderModuleFunctions : public CRTPBase<T>
{};

template <typename T>
struct ShaderModuleGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(ShaderModule, ShaderModuleFunctions, ShaderModuleGroupFunctions);
}

}    //  namespace renderer::vk
