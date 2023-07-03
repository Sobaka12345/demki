#include "queue.hpp"
#include "device.hpp"

namespace vk { namespace handles {

Queue::Queue(Queue&& other) noexcept
	: Handle(std::move(other))
{}

Queue& Queue::operator=(Queue&& other) noexcept
{
	Handle::operator=(std::move(other));

	return *this;
}

Queue::Queue(
    const Device& device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkHandleType* handlePtr)
	: Handle(handlePtr)
{
	create(vkGetDeviceQueue, device, queueFamilyIndex, queueIndex);
}

Queue::~Queue() {}

VkResult Queue::waitIdle() const
{
	return vkQueueWaitIdle(handle());
}

VkResult Queue::submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) const
{
	return vkQueueSubmit(handle(), submitCount, pSubmits, fence);
}

VkResult Queue::presentKHR(const VkPresentInfoKHR* pPresentInfo) const
{
	return vkQueuePresentKHR(handle(), pPresentInfo);
}

}}    //  namespace vk::handles
