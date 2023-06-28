#include "descriptor_pool.hpp"

#include "creators.hpp"

namespace vk {

DescriptorPool::DescriptorPool(const Device& device,
    uint32_t capacity, std::span<const VkDescriptorPoolSize> poolSizes)
    : m_device(device)
{
    VkDescriptorPoolCreateInfo poolInfo = create::descriptorPoolCreateInfo(2, poolSizes);

    ASSERT(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS,
        "failed to create descriptor pool!");
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
}

std::shared_ptr<DescriptorSet> DescriptorPool::allocateSet(
    VkDescriptorSetLayout layout, VkPipelineLayout pipelineLayout)
{
    const auto allocInfo =
        create::descriptorSetAllocateInfo(m_descriptorPool, &layout, 1);

    return std::shared_ptr<DescriptorSet>(
        new DescriptorSet(m_device, pipelineLayout, allocInfo));
}

}
