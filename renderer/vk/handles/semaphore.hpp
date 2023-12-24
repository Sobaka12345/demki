#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(SemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkSemaphoreCreateFlags, flags)
END_DECLARE_VKSTRUCT()

class Device;

class Semaphore : public Handle<VkSemaphore>
{
    HANDLE(Semaphore);

public:
    Semaphore(const Semaphore& other) = delete;
    Semaphore(Semaphore&& other) noexcept;
    Semaphore(const Device& device,
        SemaphoreCreateInfo createInfo = SemaphoreCreateInfo{}) noexcept;
    virtual ~Semaphore();

protected:
    Semaphore(
        const Device& device, SemaphoreCreateInfo createInfo, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
