#include "fence.hpp"

#include "device.hpp"

namespace vk {

Fence::Fence(Fence &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Fence::Fence(const Device &device, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{}

Fence::Fence(const Device &device, FenceCreateInfo createInfo, VkHandleType *handlePtr)
    : Fence(device, handlePtr)
{
    ASSERT(create(vkCreateFence, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create fence");
}

Fence::~Fence()
{
    destroy(vkDestroyFence, m_device, handle(), nullptr);
}

}    //  namespace vk
