#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_UNTYPED_VKSTRUCT(DescriptorPoolSize)
    VKSTRUCT_PROPERTY(VkDescriptorType, type)
    VKSTRUCT_PROPERTY(uint32_t, descriptorCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(DescriptorPoolCreateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorPoolCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, maxSets)
    VKSTRUCT_PROPERTY(uint32_t, poolSizeCount)
    VKSTRUCT_PROPERTY(const DescriptorPoolSize*, pPoolSizes)
END_DECLARE_VKSTRUCT()

template <typename T>
struct DescriptorPoolFunctions : public CRTPBase<T>
{
    CREATE_FUNC(DescriptorPool, Device);
    DESTROY_FUNC(DescriptorPool, Device);
};

template <typename T>
struct DescriptorPoolGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(DescriptorPool, DescriptorPoolFunctions, DescriptorPoolGroupFunctions);
}

}    //  namespace renderer::vk
