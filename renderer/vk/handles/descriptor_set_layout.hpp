#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <unordered_map>

namespace vk { namespace handles {

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

class Device;

class DescriptorSetLayout : public Handle<VkDescriptorSetLayout>
{
    HANDLE(DescriptorSetLayout);

public:
    DescriptorSetLayout(const DescriptorSetLayout& other) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout(const Device& device, DescriptorSetLayoutCreateInfo info) noexcept;
    virtual ~DescriptorSetLayout();

    handles::DescriptorSetLayoutBinding binding(int32_t bindingId) const;

protected:
    DescriptorSetLayout(
        const Device& device, DescriptorSetLayoutCreateInfo info, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
    std::unordered_map<int32_t, handles::DescriptorSetLayoutBinding> m_bindings;
};

}}    //  namespace vk::handles
