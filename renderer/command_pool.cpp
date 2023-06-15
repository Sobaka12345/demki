#include "command_pool.hpp"

#include "creators.hpp"

#include <stdexcept>

namespace vk {

CommandPool::CommandPool(const Device &device, VkCommandPoolCreateInfo poolInfo)
    : m_device(device)
{
    ASSERT(vkCreateCommandPool(device, &poolInfo, nullptr, &m_handle) == VK_SUCCESS,
        "failed to create command pool!");
}

CommandPool::CommandPool(CommandPool &&other)
    : HandleBase(std::move(other))
    , m_device(other.m_device)
{
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(m_device, m_handle, nullptr);
}

CommandBuffer CommandPool::allocateBuffer(VkCommandBufferLevel level)
{
    VkCommandBuffer buffer;
    const auto allocateInfo = create::commandBufferAllocateInfo(handle(), level, 1);
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, &buffer) == VK_SUCCESS,
        "failed to allocate command buffer!");
    return CommandBuffer{*this, buffer};
}

std::vector<CommandBuffer> CommandPool::allocateBuffers(uint32_t count, VkCommandBufferLevel level)
{
    std::vector<CommandBuffer> result;
    result.reserve(count);

    std::vector<VkCommandBuffer> commandBuffers;
    const auto allocateInfo = create::commandBufferAllocateInfo(handle(), level, count);
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, commandBuffers.data()) == VK_SUCCESS,
        "failed to allocate command buffer!");

    for (VkCommandBuffer cb : commandBuffers) result.emplace_back(CommandBuffer{*this, cb});

    return result;
}

}
