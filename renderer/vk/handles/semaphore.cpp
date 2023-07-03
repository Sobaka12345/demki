#include "semaphore.hpp"

#include "device.hpp"

namespace vk { namespace handles {

Semaphore::Semaphore(Semaphore &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Semaphore::Semaphore(const Device &device, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{}

Semaphore::Semaphore(const Device &device, SemaphoreCreateInfo createInfo, VkHandleType *handlePtr)
    : Semaphore(device, handlePtr)
{
    ASSERT(create(vkCreateSemaphore, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create Semaphore");
}

Semaphore::~Semaphore()
{
    destroy(vkDestroySemaphore, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
