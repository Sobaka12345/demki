#include "descriptor_pool.hpp"

#include "device.hpp"
#include "descriptor_set.hpp"
#include "descriptor_set_layout.hpp"

namespace vk { namespace handles {

DescriptorPool::DescriptorPool(DescriptorPool&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

DescriptorPool::DescriptorPool(
    const Device& device, DescriptorPoolCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    m_sets.resize(createInfo.maxSets());
    ASSERT(create(vkCreateDescriptorPool, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
    destroy(vkDestroyDescriptorPool, m_device, handle(), nullptr);
}

std::shared_ptr<DescriptorSet> DescriptorPool::allocateSet(const DescriptorSetLayout& layout)
{
    const auto allocInfo =
        DescriptorSetAllocateInfo{}
            .descriptorPool(handle())
            .pSetLayouts(layout.handlePtr())
            .descriptorSetCount(1);

    auto iter = std::find_if(m_sets.begin(), m_sets.end(), [](const auto& x) {
        return x.expired();
    });

    //  TO DO
    ASSERT(iter != m_sets.end(), "pool reached max number of sets");

    auto result = std::make_shared<DescriptorSet>(m_device, allocInfo);
    *iter = result;

    return result;
}

}}    //  namespace vk::handles
