#include "descriptor_set_layout.hpp"

#include "device.hpp"

namespace vk { namespace handles {

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_bindings(std::move(other.m_bindings))
{}

DescriptorSetLayout::DescriptorSetLayout(
    const Device& device, DescriptorSetLayoutCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    for (size_t i = 0; i < createInfo.bindingCount(); ++i)
    {
        m_bindings[createInfo.pBindings()[i].binding] = createInfo.pBindings()[i];
    }

    ASSERT(create(vkCreateDescriptorSetLayout, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create descriptor set layout!");
}

handles::DescriptorSetLayoutBinding DescriptorSetLayout::binding(int32_t bindingId) const
{
    return m_bindings.at(bindingId);
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    destroy(vkDestroyDescriptorSetLayout, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
