#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(FenceCreateInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkFenceCreateFlags, flags)
END_DECLARE_VKSTRUCT()

template <typename T>
struct FenceFunctions : public CRTPBase<T>
{
    CREATE_FUNC(Fence, Device)
    DESTROY_FUNC(Fence, Device)
};

template <typename T>
struct FenceGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Fence, FenceFunctions, FenceGroupFunctions);
}

}    //  namespace renderer::vk
