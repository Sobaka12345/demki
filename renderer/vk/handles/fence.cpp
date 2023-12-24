#include "fence.hpp"

#include "device.hpp"

namespace vk { namespace handles {

Fence::Fence(Fence&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

Fence::Fence(const Device& device, FenceCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateFence, device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create fence");
}

Fence::Fence(const Device& device, FenceCreateInfo createInfo) noexcept
    : Fence(device, std::move(createInfo), nullptr)
{}

Fence::~Fence()
{
    destroy(vkDestroyFence, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
