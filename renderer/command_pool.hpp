#pragma once

#include "device.hpp"
#include "command_buffer.hpp"

#include <memory>

namespace vk {

class CommandPool
{
public:
    CommandPool(const Device& device, VkCommandPoolCreateInfo poolInfo);
    CommandPool(CommandPool&& other);
    CommandPool(const CommandPool& other) = delete;
    ~CommandPool();

    CommandBuffer allocateBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    std::vector<CommandBuffer> allocateBuffers(uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

private:
    const Device& m_device;
    VkCommandPool m_commandPool;
};

}
