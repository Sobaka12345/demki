#pragma once

#include "utils.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(FenceCreateInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkFenceCreateFlags, flags)
END_DECLARE_VKSTRUCT()

class Device;

class Fence : public Handle<VkFence>
{
public:
    Fence(const Fence& other) = delete;
    Fence(Fence&& other);
    Fence(const Device& device, VkHandleType* handlePtr);
    Fence(const Device& device, FenceCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~Fence();

private:
    const Device& m_device;
};

}    //  namespace vk
