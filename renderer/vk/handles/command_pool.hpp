#pragma once

#include "device.hpp"
#include "command_buffer.hpp"

#include <memory>

namespace vk { namespace handles {

class CommandPool : public Handle<VkCommandPool>
{
public:
    CommandPool(
        const Device& device, VkCommandPoolCreateInfo poolInfo, VkHandleType* handlePtr = nullptr);
    CommandPool(CommandPool&& other) noexcept;
    CommandPool(const CommandPool& other) = delete;
    ~CommandPool();

    CommandBuffer allocateBuffer(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
    HandleVector<CommandBuffer> allocateBuffers(uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
