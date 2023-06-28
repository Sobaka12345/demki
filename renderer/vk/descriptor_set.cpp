#include "descriptor_set.hpp"

#include "device.hpp"

namespace vk {

DescriptorSet::DescriptorSet(DescriptorSet&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_pipelineLayout(other.m_pipelineLayout)
{}

DescriptorSet::DescriptorSet(const Device& device, VkPipelineLayout pipelineLayout,
    DescriptorSetAllocateInfo allocInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_pipelineLayout(pipelineLayout)
{
    ASSERT(create(vkAllocateDescriptorSets, m_device, &allocInfo) == VK_SUCCESS,
        "failed to allocate descriptor sets!");
    setOwner(false);
}

DescriptorSet::~DescriptorSet()
{
    //vkFreeDescriptorSets(m_device, )
}

void DescriptorSet::bind(VkCommandBuffer commandBuffer, uint32_t offset) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout, 0, 1, handlePtr(), 1, &offset);
}

void DescriptorSet::write(std::span<const Write> writes)
{
    std::vector<WriteDescriptorSet> writeDescriptorSets;
    for (auto& write : writes)
    {
        writeDescriptorSets.push_back(WriteDescriptorSet()
            .dstSet(handle())
            .dstBinding(write.layoutBinding.binding)
            .descriptorType(write.layoutBinding.descriptorType)
            .descriptorCount(write.layoutBinding.descriptorCount)
            .pImageInfo(write.imageInfo.has_value() ? &write.imageInfo.value() : nullptr)
            .pBufferInfo(write.bufferInfo.has_value() ? &write.bufferInfo.value() : nullptr)
        );
    }

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writeDescriptorSets.size()),
        writeDescriptorSets.data(), 0, nullptr);
}

}
