#include "descriptor_set_layout.hpp"

#include "device.hpp"

namespace vk { namespace handles {

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_bindings(std::move(other.m_bindings))
{}

DescriptorSetLayout::DescriptorSetLayout(const Device& device,
    DescriptorSetLayoutCreateInfo createInfo,
    VkHandleType* handlePtr) noexcept
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

DescriptorSetLayout::DescriptorSetLayout(const Device& device,
                                         DescriptorSetLayoutCreateInfo createInfo) noexcept
    : DescriptorSetLayout(device, std::move(createInfo), nullptr)
{}

DescriptorSetLayout::~DescriptorSetLayout()
{
    destroy(vkDestroyDescriptorSetLayout, m_device, handle(), nullptr);
}

handles::DescriptorSetLayoutBinding DescriptorSetLayout::binding(int32_t bindingId) const
{
    return m_bindings.at(bindingId);
}

}}    //  namespace vk::handles
