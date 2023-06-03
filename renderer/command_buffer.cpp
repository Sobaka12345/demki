#include "command_buffer.hpp"


namespace vk {

CommandBuffer::CommandBuffer(const CommandPool& pool, VkCommandBuffer commandBuffer)
    : m_pool(pool)
    , m_commandBuffer(commandBuffer)
{

}

CommandBuffer::CommandBuffer(CommandBuffer &&other)
    : m_pool(other.m_pool)
{

}

}
