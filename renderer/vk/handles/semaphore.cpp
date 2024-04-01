#include "semaphore.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {

Semaphore::Semaphore(Semaphore&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Semaphore::Semaphore(
    const Device& device, SemaphoreCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateSemaphore, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create Semaphore");
}

Semaphore::Semaphore(const Device& device, SemaphoreCreateInfo createInfo) noexcept
    : Semaphore(device, std::move(createInfo), nullptr)
{}

Semaphore::~Semaphore()
{
    destroy(vkDestroySemaphore, m_device, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles
