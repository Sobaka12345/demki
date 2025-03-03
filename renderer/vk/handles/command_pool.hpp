#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(CommandPoolCreateInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandPoolCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndex)
END_DECLARE_VKSTRUCT();


template <typename T>
struct CommandPoolFunctions : public CRTPBase<T>
{
    CREATE_FUNC(CommandPool, Device)
    DESTROY_FUNC(CommandPool, Device)
};

template <typename T>
struct CommandPoolGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(CommandPool, CommandPoolFunctions, CommandPoolGroupFunctions);
}

}    //  namespace renderer::vk
