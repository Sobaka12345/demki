#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(SubmitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(uint32_t, waitSemaphoreCount)
    VKSTRUCT_PROPERTY(const VkSemaphore*, pWaitSemaphores)
    VKSTRUCT_PROPERTY(const VkPipelineStageFlags*, pWaitDstStageMask)
    VKSTRUCT_PROPERTY(uint32_t, commandBufferCount)
    VKSTRUCT_PROPERTY(const VkCommandBuffer*, pCommandBuffers)
    VKSTRUCT_PROPERTY(uint32_t, signalSemaphoreCount)
    VKSTRUCT_PROPERTY(const VkSemaphore*, pSignalSemaphores)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PresentInfoKHR, VK_STRUCTURE_TYPE_PRESENT_INFO_KHR)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(uint32_t, waitSemaphoreCount)
    VKSTRUCT_PROPERTY(const VkSemaphore*, pWaitSemaphores)
    VKSTRUCT_PROPERTY(uint32_t, swapchainCount)
    VKSTRUCT_PROPERTY(const VkSwapchainKHR*, pSwapchains)
    VKSTRUCT_PROPERTY(const uint32_t*, pImageIndices)
    VKSTRUCT_PROPERTY(VkResult*, pResults)
END_DECLARE_VKSTRUCT()

template <typename T>
struct QueueFunctions : public CRTPBase<T>
{
    static constexpr inline VkQueue get(
        VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex) noexcept
    {
        VkQueue result = VK_NULL_HANDLE;
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &result);
        return result;
    }
};

template <typename T>
struct QueueGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Queue, QueueFunctions, QueueGroupFunctions);
}

}    //  namespace renderer::vk
