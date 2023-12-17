#include "descriptor_pool.hpp"

#include "device.hpp"
#include "descriptor_set.hpp"
#include "descriptor_set_layout.hpp"

namespace vk { namespace handles {

DescriptorPool::DescriptorPool(DescriptorPool&& other)
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_maxSetCount(other.m_maxSetCount)
{}

DescriptorPool::DescriptorPool(
    const Device& device, DescriptorPoolCreateInfo createInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_maxSetCount(createInfo.maxSets())
    , m_device(device)
{
    ASSERT(create(vkCreateDescriptorPool, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
    std::for_each(m_allocatedSets.begin(), m_allocatedSets.end(), [](DescriptorSet* set) {
        set->setOwner(false);
    });
    destroy(vkDestroyDescriptorPool, m_device, handle(), nullptr);
}

std::shared_ptr<DescriptorSet> DescriptorPool::allocateSet(const DescriptorSetLayout& layout)
{
    const auto allocInfo =
        DescriptorSetAllocateInfo{}
            .descriptorPool(handle())
            .pSetLayouts(layout.handlePtr())
            .descriptorSetCount(1);

    auto set = std::shared_ptr<DescriptorSet>(new DescriptorSet(m_device, allocInfo),
        [this](DescriptorSet* set) {
            std::default_delete<DescriptorSet>{}(set);
            m_allocatedSets.erase(set);
        });
    m_allocatedSets.insert(set.get());

    return set;
}

std::vector<std::shared_ptr<DescriptorSets>> DescriptorPool::allocateSets(
    std::span<const DescriptorSetLayout> layouts)
{
    ASSERT(false, "not implemented");
    return {};
}

bool DescriptorPool::isFull() const
{
    return m_maxSetCount == m_allocatedSets.size();
}

bool DescriptorPool::isEmpty() const
{
    return m_allocatedSets.empty();
}

}}    //  namespace vk::handles
