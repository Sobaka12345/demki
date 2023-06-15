#pragma once

#include "device.hpp"
#include "descriptor_set.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <span>

namespace vk {

class DescriptorPool
{
public:
    DescriptorPool() = delete;
    DescriptorPool(const Device& device,
        uint32_t capacity, std::span<const VkDescriptorPoolSize> poolSizes);
    ~DescriptorPool();

    operator VkDescriptorPool() const { return m_descriptorPool; }

    std::shared_ptr<DescriptorSet> allocateSet(VkDescriptorSetLayout layout,
        VkPipelineLayout pipelineLayout);

private:
    const Device& m_device;
    VkDescriptorPool m_descriptorPool;
};

}
