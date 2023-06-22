#pragma once
#include "handle.hpp"

#include <vulkan/vulkan.h>

namespace vk {

class Device;
class CommandPool;

class CommandBuffer : public Handle<VkCommandBuffer>
{

public:
    CommandBuffer(const CommandBuffer& other) = delete;
    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer(const Device& device, VkHandleType* handlePtr = nullptr);
    CommandBuffer(const Device& device, const CommandPool& pool,
        VkCommandBufferLevel level, VkHandleType* handlePtr = nullptr);
    ~CommandBuffer();

private:
    const Device& m_device;
};

}
