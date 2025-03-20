#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

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

template <typename T>
struct DescriptorSetFunctions : public CRTPBase<T>
{
    static constexpr inline void allocate(VkDevice device,
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet* pDescriptorSets) noexcept
    {
        ASSERT(vkAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets) == VK_SUCCESS);
    }

    static constexpr inline void allocate(VkDevice device,
        DescriptorSetAllocateInfo allocateInfo,
        VkDescriptorSet* pDescriptorSets) noexcept
    {
        return allocate(device, &allocateInfo, pDescriptorSets);
    }

    static constexpr inline void free(VkDevice device,
        VkDescriptorPool descriptorPool,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets) noexcept
    {
        vkFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    }
};

template <typename T>
struct DescriptorSetGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(DescriptorSet, DescriptorSetFunctions, DescriptorSetGroupFunctions);
}

}    //  namespace renderer::vk
