#pragma once

#include "device.hpp"
#include "command_buffer.hpp"

#include <memory>

namespace vk {

class CommandPool : public Handle<VkCommandPool>
{
public:
    CommandPool(const Device& device, VkCommandPoolCreateInfo poolInfo, VkHandleType* handlePtr = nullptr);
    CommandPool(CommandPool&& other) noexcept;
    CommandPool(const CommandPool& other) = delete;
    ~CommandPool();

    CommandBuffer allocateBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    HandleVector<CommandBuffer> allocateBuffers(uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

private:
    const Device& m_device;
};

}
