#pragma once

#include "utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DescriptorSetLayoutCreateInfo,
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDescriptorSetLayoutCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, bindingCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayoutBinding*, pBindings)
END_DECLARE_VKSTRUCT()

class Device;

class DescriptorSetLayout : public Handle<VkDescriptorSetLayout>
{
public:
    DescriptorSetLayout(const DescriptorSetLayout& other) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& other);
    DescriptorSetLayout(const Device& device,
        DescriptorSetLayoutCreateInfo info,
        VkHandleType* handlePtr = nullptr);

    ~DescriptorSetLayout();

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
