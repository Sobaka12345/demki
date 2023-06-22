#include "command_pool.hpp"

#include "creators.hpp"

#include <stdexcept>

namespace vk {

CommandPool::CommandPool(const Device &device, VkCommandPoolCreateInfo poolInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateCommandPool, device, &poolInfo, nullptr) == VK_SUCCESS,
        "failed to create command pool!");
}

CommandPool::CommandPool(CommandPool &&other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{
}

CommandPool::~CommandPool()
{
    destroy(vkDestroyCommandPool, m_device, handle(), nullptr);
}

CommandBuffer CommandPool::allocateBuffer(VkCommandBufferLevel level)
{
    const auto allocateInfo = create::commandBufferAllocateInfo(handle(), level, 1);
    VkCommandBuffer* buffer = new VkCommandBuffer;
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, buffer) == VK_SUCCESS,
        "failed to allocate command buffer!");
    CommandBuffer result{ m_device, buffer };
    result.setOwner(true);
    return result;
}

HandleVector<CommandBuffer> CommandPool::allocateBuffers(uint32_t count, VkCommandBufferLevel level)
{
    HandleVector<CommandBuffer> result;
    result.resize(count, m_device);

    const auto allocateInfo = create::commandBufferAllocateInfo(handle(), level, count);
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, result.handleData()) == VK_SUCCESS,
        "failed to allocate command buffer!");

    return result;
}

}
