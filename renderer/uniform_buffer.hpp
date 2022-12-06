#pragma once

#include "buffer.hpp"

namespace vk {

class UniformBuffer: public Buffer
{
public:
    UniformBuffer(VkDevice device, VkDeviceSize size, VkSharingMode sharingMode);

    bool persistentMapMemory();
    void writeInMapped();

private:
    void* m_mappedMemory;
};

}