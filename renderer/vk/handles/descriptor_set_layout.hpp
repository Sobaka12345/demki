#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(DescriptorSetLayoutCreateInfo,
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorSetLayoutCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, bindingCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayoutBinding*, pBindings)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorSetLayoutBinding)
    VKSTRUCT_PROPERTY(uint32_t, binding)
    VKSTRUCT_PROPERTY(VkDescriptorType, descriptorType)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
    VKSTRUCT_PROPERTY(VkShaderStageFlags, stageFlags)
    VKSTRUCT_PROPERTY(const VkSampler*, pImmutableSamplers)
END_DECLARE_VKSTRUCT()

template <typename T>
struct DescriptorSetLayoutFunctions : public CRTPBase<T>
{
    CREATE_FUNC(DescriptorSetLayout, Device);
    DESTROY_FUNC(DescriptorSetLayout, Device);
};

template <typename T>
struct DescriptorSetLayoutGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(
    DescriptorSetLayout, DescriptorSetLayoutFunctions, DescriptorSetLayoutGroupFunctions);
}

}    //  namespace renderer::vk
