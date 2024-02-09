#include "descriptor_pool.hpp"

#include "device.hpp"
#include "descriptor_set.hpp"
#include "descriptor_set_layout.hpp"

namespace renderer::vk { namespace handles {

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_maxSetCount(other.m_maxSetCount)
    , m_currentSetCount(other.m_currentSetCount)
{}

DescriptorPool::DescriptorPool(
    const Device& device, DescriptorPoolCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_maxSetCount(createInfo.maxSets())
    , m_device(device)
    , m_currentSetCount(0)
{
    ASSERT(create(vkCreateDescriptorPool, m_device, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create descriptor pool");
}

DescriptorPool::DescriptorPool(const Device& device, DescriptorPoolCreateInfo createInfo) noexcept
    : DescriptorPool(device, std::move(createInfo), nullptr)
{}

DescriptorPool::~DescriptorPool()
{
    destroy(vkDestroyDescriptorPool, m_device, handle(), nullptr);
}

std::shared_ptr<DescriptorSet> DescriptorPool::allocateSet(const DescriptorSetLayout& layout)
{
    auto set = std::shared_ptr<DescriptorSet>(new DescriptorSet(m_device, this, layout),
        [this](DescriptorSet* set) {
            if (set->m_pool.isAlive()) --m_currentSetCount;

            std::default_delete<DescriptorSet>{}(set);
        });
    ++m_currentSetCount;

    return set;
}

std::vector<std::shared_ptr<DescriptorSet>> DescriptorPool::allocateSets(
    const HandleVector<DescriptorSetLayout>& layouts)
{
    m_currentSetCount += layouts.size();
    return DescriptorSet::createShared(m_device, this, layouts, [this](DescriptorSet* set) {
        if (set->m_pool.isAlive()) --m_currentSetCount;

        std::default_delete<DescriptorSet>{}(set);
    });
}

bool DescriptorPool::isFull() const
{
    return m_maxSetCount == m_currentSetCount;
}

bool DescriptorPool::isEmpty() const
{
    return !m_currentSetCount;
}


}}    //  namespace renderer::vk::handles
