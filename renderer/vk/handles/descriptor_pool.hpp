#pragma once

#include "utils.hpp"
#include "descriptor_set.hpp"

#include <cstdint>
#include <unordered_set>
#include <span>
#include <memory>

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
    std::vector<std::shared_ptr<DescriptorSets>> allocateSets(
        std::span<const DescriptorSetLayout> layouts);

    bool isFull() const;
    bool isEmpty() const;

private:
    const Device& m_device;
    const uint32_t m_maxSetCount;
    std::unordered_set<DescriptorSet*> m_allocatedSets;
};

}}    //  namespace vk::handles
