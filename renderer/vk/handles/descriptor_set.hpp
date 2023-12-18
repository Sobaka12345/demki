#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <span>
#include <optional>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DescriptorSetAllocateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO)
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
class DescriptorPool;

class DescriptorSet : public Handle<VkDescriptorSet>
{
    friend class DescriptorSets;

public:
    struct Write
    {
        std::optional<VkDescriptorImageInfo> imageInfo;
        std::optional<VkDescriptorBufferInfo> bufferInfo;
        VkDescriptorSetLayoutBinding layoutBinding;
    };

    DescriptorSet(const DescriptorSet& other) = delete;
    DescriptorSet(DescriptorSet&& other);
    ~DescriptorSet();

    void write(std::span<const Write> writes);
    void write(std::span<const WriteDescriptorSet> writes);

private:
    DescriptorSet(const Device& device,
        HandlePtr<DescriptorPool> pool,
        std::span<const VkDescriptorSetLayout> setLayouts,
        VkHandleType* handlePtr = nullptr);

    friend class DescriptorPool;

protected:
    const Device& m_device;
    HandlePtr<DescriptorPool> m_pool;
};

class DescriptorSets : public HandleVector<DescriptorSet>
{
public:
    using HandleVector::HandleVector;

    void write(std::span<const WriteDescriptorSet> writes);
};

}}    //  namespace vk::handles
