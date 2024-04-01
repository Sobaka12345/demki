#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace renderer::vk { namespace handles {

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

class Device;

class Queue : public Handle<VkQueue>
{
    HANDLE(Queue);

public:
    Queue(const Queue& other) = delete;
	Queue(Queue&& other) noexcept;
	Queue& operator=(Queue&& other) noexcept;
    Queue(const Device& device, uint32_t queueFamilyIndex, uint32_t queueIndex) noexcept;
    virtual ~Queue();

	VkResult waitIdle() const;
    VkResult submit(uint32_t submitCount, const SubmitInfo* pSubmits, VkFence fence) const;
    VkResult presentKHR(PresentInfoKHR presentInfo) const;

protected:
    Queue(const Device& device,
        uint32_t queueFamilyIndex,
        uint32_t queueIndex,
        VkHandleType* handlePtr) noexcept;
};

}}    //  namespace renderer::vk::handles
