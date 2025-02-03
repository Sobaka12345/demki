#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(SemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkSemaphoreCreateFlags, flags)
END_DECLARE_VKSTRUCT()

template <typename T>
struct SemaphoreFunctions : public CRTPBase<T>
{};

template <typename T>
struct SemaphoreGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Semaphore, SemaphoreFunctions, SemaphoreGroupFunctions);
}

}    //  namespace renderer::vk
