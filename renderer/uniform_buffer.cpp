#include "uniform_buffer.hpp"

namespace vk {

UniformBuffer::UniformBuffer(VkDevice device, VkDeviceSize size, VkSharingMode sharingMode)
    : Buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sharingMode)
    , m_mappedMemory(nullptr)
{
}

bool UniformBuffer::persistentMapMemory()
{
    return mapMemory(&m_mappedMemory, 0, 0);
}

}