#pragma once

#include "handle.hpp"

namespace vk {

class Device;

class Queue : public Handle<VkQueue>
{
public:
	Queue(Queue&& other) noexcept;
	Queue& operator=(Queue&& other) noexcept;
	Queue(const Queue& other) = delete;
	Queue(const Device& device, uint32_t queueFamilyIndex,
		uint32_t queueIndex, VkHandleType* handlePtr = nullptr);
	~Queue();

	VkResult waitIdle() const;
	VkResult submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) const;
	VkResult presentKHR(const VkPresentInfoKHR* pPresentInfo) const;
};

}