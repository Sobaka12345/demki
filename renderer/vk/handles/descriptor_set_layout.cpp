#include "descriptor_set_layout.hpp"

#include "device.hpp"

namespace vk { namespace handles {

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

DescriptorSetLayout::DescriptorSetLayout(
    const Device& device, DescriptorSetLayoutCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateDescriptorSetLayout, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create descriptor set layout!");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    destroy(vkDestroyDescriptorSetLayout, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
