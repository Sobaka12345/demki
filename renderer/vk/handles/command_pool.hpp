#pragma once

#include "device.hpp"
#include "command_buffer.hpp"

#include <memory>

namespace renderer::vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(CommandPoolCreateInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandPoolCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndex)
END_DECLARE_VKSTRUCT()

class CommandPool : public Handle<VkCommandPool>
{
    HANDLE(CommandPool);

public:
    CommandPool(const CommandPool& other) = delete;
    CommandPool(const Device& device, CommandPoolCreateInfo poolInfo) noexcept;
    CommandPool(CommandPool&& other) noexcept;
    ~CommandPool();

    CommandBuffer allocateBuffer(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
    HandleVector<CommandBuffer> allocateBuffers(uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

protected:
    CommandPool(
        const Device& device, CommandPoolCreateInfo poolInfo, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace renderer::vk::handles
