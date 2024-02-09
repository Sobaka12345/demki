#include "command_pool.hpp"

namespace renderer::vk { namespace handles {

CommandPool::CommandPool(
    const Device& device, CommandPoolCreateInfo poolInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateCommandPool, device, &poolInfo, nullptr) == VK_SUCCESS,
        "failed to create command pool!");
}

CommandPool::CommandPool(const Device& device, CommandPoolCreateInfo poolInfo) noexcept
    : CommandPool(device, std::move(poolInfo), nullptr)
{}

CommandPool::CommandPool(CommandPool&& other) noexcept
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
    result.resize(count, m_device, *this, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    const auto allocateInfo =
        CommandBufferAllocateInfo{}.commandBufferCount(count).commandPool(handle()).level(level);
    ASSERT(vkAllocateCommandBuffers(m_device, &allocateInfo, result.handleData()) == VK_SUCCESS,
        "failed to allocate command buffer!");

    return result;
}

}}    //  namespace renderer::vk::handles
