#pragma once

#include "utils.hpp"

#include <span>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DescriptorSetAllocateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO)
    VKSTRUCT_PROPERTY(VkStructureType, sType)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorPool, descriptorPool)
    VKSTRUCT_PROPERTY(uint32_t, descriptorSetCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayout*, pSetLayouts)
END_DECLARE_VKSTRUCT();

BEGIN_DECLARE_VKSTRUCT(WriteDescriptorSet, VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorSet, dstSet)
    VKSTRUCT_PROPERTY(uint32_t, dstBinding)
    VKSTRUCT_PROPERTY(uint32_t, dstArrayElement)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
    VKSTRUCT_PROPERTY(VkDescriptorType, descriptorType)
    VKSTRUCT_PROPERTY(const VkDescriptorImageInfo*, pImageInfo)
    VKSTRUCT_PROPERTY(const VkDescriptorBufferInfo*, pBufferInfo)
    VKSTRUCT_PROPERTY(const VkBufferView*, pTexelBufferView)
END_DECLARE_VKSTRUCT()

class Device;

class DescriptorSet : public Handle<VkDescriptorSet>
{
public:
    struct Write
    {
        std::optional<VkDescriptorImageInfo> imageInfo;
        std::optional<VkDescriptorBufferInfo> bufferInfo;
        VkDescriptorSetLayoutBinding layoutBinding;
    };

    DescriptorSet(const DescriptorSet& other) = delete;
    DescriptorSet(DescriptorSet&& other);

    DescriptorSet(const Device& device,
        VkPipelineLayout pipelineLayout,
        DescriptorSetAllocateInfo allocInfo,
        VkHandleType* handlePtr = nullptr);

    ~DescriptorSet();

    virtual void bind(VkCommandBuffer commandBuffer, uint32_t offset) const;

    void write(std::span<const Write> writes);

protected:
    const Device& m_device;
    VkPipelineLayout m_pipelineLayout;
};

}}    //  namespace vk::handles
