#pragma once

#include "utils.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(SemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkSemaphoreCreateFlags, flags)
END_DECLARE_VKSTRUCT()

class Device;

class Semaphore : public Handle<VkSemaphore>
{
public:
    Semaphore(const Semaphore& other) = delete;
    Semaphore(Semaphore&& other);
    Semaphore(const Device& device, VkHandleType* handlePtr);
    Semaphore(
        const Device& device, SemaphoreCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~Semaphore();

private:
    const Device& m_device;
};

}    //  namespace vk
