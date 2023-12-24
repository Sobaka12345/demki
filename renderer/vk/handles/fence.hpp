#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(FenceCreateInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkFenceCreateFlags, flags)
END_DECLARE_VKSTRUCT()

class Device;

class Fence : public Handle<VkFence>
{
    HANDLE(Fence);

public:
    Fence(const Fence& other) = delete;
    Fence(Fence&& other) noexcept;
    Fence(const Device& device, FenceCreateInfo createInfo = FenceCreateInfo{}) noexcept;
    virtual ~Fence();

protected:
    Fence(const Device& device, FenceCreateInfo createInfo, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
