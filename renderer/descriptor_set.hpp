#pragma once

#include "device.hpp"
#include "creators.hpp"

#include <vulkan/vulkan_core.h>

#include <span>
#include <vector>
#include <type_traits>

namespace vk {

class DescriptorSet
{
public:
    struct Write
    {
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorSetLayoutBinding layoutBinding;
    };

    DescriptorSet(const Device& device, VkPipelineLayout pipelineLayout,
        VkDescriptorSetAllocateInfo allocInfo)
        : m_device(device)
        , m_pipelineLayout(pipelineLayout)
    {
        if (vkAllocateDescriptorSets(m_device, &allocInfo, &m_set) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
    }

    virtual ~DescriptorSet()
    {
        // TO DO: FREE DESC SETS
    }

    virtual void bind(VkCommandBuffer commandBuffer, uint32_t offset) const
    {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipelineLayout, 0, 1, &m_set, 1, &offset);
    }

    void write(std::span<const Write> writes)
    {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets(writes.size());
        for (auto& write : writes)
        {
            writeDescriptorSets.push_back(
                create::writeDescriptorSet(m_set, write.layoutBinding.binding,
                    write.layoutBinding.descriptorType, 1, &write.bufferInfo)
            );
        }

        vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writeDescriptorSets.size()),
            writeDescriptorSets.data(), 0, nullptr);
    }

protected:
    const Device& m_device;
    VkDescriptorSet m_set;
    VkPipelineLayout m_pipelineLayout;
};

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(const Device& device, VkDescriptorSetLayoutCreateInfo info)
        : m_device(device)
    {
        if (vkCreateDescriptorSetLayout(m_device, &info, nullptr, &m_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    operator VkDescriptorSetLayout() const { return m_layout; }

    ~DescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
    }

private:
    const Device& m_device;
    VkDescriptorSetLayout m_layout;
};

}
