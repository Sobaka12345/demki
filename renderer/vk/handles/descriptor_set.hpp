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
{};

template <typename T>
struct DescriptorSetGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE_FULL(DescriptorSet,
    vkAllocateDescriptorSets,
    vkFreeDescriptorSets,
    DescriptorSetFunctions,
    DescriptorSetGroupFunctions);
}

}    //  namespace renderer::vk
