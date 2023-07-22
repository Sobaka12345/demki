#pragma once

#include "utils.hpp"
#include "descriptor_set.hpp"

#include <cstdint>
#include <span>

namespace vk { namespace handles {

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorPoolSize)
    VKSTRUCT_PROPERTY(VkDescriptorType, type)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(DescriptorPoolCreateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorPoolCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, maxSets)
    VKSTRUCT_PROPERTY(uint32_t, poolSizeCount)
    VKSTRUCT_PROPERTY(const VkDescriptorPoolSize*, pPoolSizes)
END_DECLARE_VKSTRUCT()

class Device;
class DescriptorPool;
class DescriptorSetLayout;
class PipelineLayout;

class DescriptorPool : public Handle<VkDescriptorPool>
{
public:
    DescriptorPool(const DescriptorPool& other) = delete;
    DescriptorPool(DescriptorPool&& other);
    DescriptorPool(const Device& device,
        DescriptorPoolCreateInfo createInfo,
        VkHandleType* handlePtr = nullptr);
    ~DescriptorPool();

    std::shared_ptr<DescriptorSet> allocateSet(const DescriptorSetLayout& layout);
    //  DescriptorSets allocateSets(const DescriptorSetLayout& layout, uint32_t count);

private:
    const Device& m_device;
    std::vector<std::weak_ptr<DescriptorSet>> m_sets;
};

}}    //  namespace vk::handles
