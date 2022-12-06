#pragma once

#include <list>
#include <vulkan/vulkan.h>

namespace vk {

class Buffer
{
public:
    class Memory
    {
        VkDeviceMemory m_memory;
    }

    Buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode);
    ~Buffer();

    VkDeviceSize size() const { return m_size; }

    VkDeviceMemory allocateMemory(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, uint32_t bindingOffset = 0);

private:
    VkBuffer m_buffer;
    VkDevice m_device;
    VkDeviceSize m_size;
    std::list<VkDeviceMemory> m_memoryList;
};

}