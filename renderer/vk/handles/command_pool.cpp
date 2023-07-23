#include "command_pool.hpp"

namespace vk { namespace handles {

CommandPool::CommandPool(
    const Device &device, CommandPoolCreateInfo poolInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateCommandPool, device, &poolInfo, nullptr) == VK_SUCCESS,
        "failed to create command pool!");
}

CommandPool::CommandPool(CommandPool &&other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

CommandPool::~CommandPool()
{
    destroy(vkDestroyCommandPool, m_device, handle(), nullptr);
}

CommandBuffer CommandPool::allocateBuffer(VkCommandBufferLevel level) const
{
    return { m_device, *this, level };
}

HandleVector<CommandBuffer> CommandPool::allocateBuffers(uint32_t count,
    VkCommandBufferLevel level) const
{
    HandleVector<CommandBuffer> result;
    result.resize(count, m_device, *this);

    const auto allocateInfo =
        CommandBufferAllocateInfo{}.commandBufferCount(count).commandPool(handle()).level(level);
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, result.handleData()) == VK_SUCCESS,
        "failed to allocate command buffer!");

    return result;
}

}}    //  namespace vk::handles
