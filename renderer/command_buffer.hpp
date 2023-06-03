#pragma once

#include <vulkan/vulkan.h>

namespace vk {

class CommandPool;

class CommandBuffer
{
    friend class CommandPool;
    CommandBuffer(const CommandPool& pool, VkCommandBuffer buffer);
    //CommandBuffer(const CommandPool& pool, VkCommandBufferAllocateInfo buffer);

public:
    CommandBuffer(CommandBuffer&& other);
    CommandBuffer(const CommandBuffer& other) = delete;

private:
    const CommandPool& m_pool;
    VkCommandBuffer m_commandBuffer;
};

}
