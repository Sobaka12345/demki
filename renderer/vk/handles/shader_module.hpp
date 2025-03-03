#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(ShaderModuleCreateInfo, VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkShaderModuleCreateFlags, flags)
    VKSTRUCT_PROPERTY(size_t, codeSize)
    VKSTRUCT_PROPERTY(const uint32_t*, pCode)
END_DECLARE_VKSTRUCT()

template <typename T>
struct ShaderModuleFunctions : public CRTPBase<T>
{
    CREATE_FUNC(ShaderModule, Device)
    DESTROY_FUNC(ShaderModule, Device)
};

template <typename T>
struct ShaderModuleGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(ShaderModule, ShaderModuleFunctions, ShaderModuleGroupFunctions);
}

}    //  namespace renderer::vk
