#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "creators.hpp"

namespace vk {
CommandBuffer::CommandBuffer(const Device& device, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
}

CommandBuffer::~CommandBuffer()
{
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

CommandBuffer::CommandBuffer(const Device& device, const CommandPool& pool,
    VkCommandBufferLevel level, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    const auto allocateInfo = create::commandBufferAllocateInfo(pool, level, 1);
    ASSERT(create(vkAllocateCommandBuffers, device, &allocateInfo) == VK_SUCCESS,
        "failed to create CommandBuffer");
}

}
