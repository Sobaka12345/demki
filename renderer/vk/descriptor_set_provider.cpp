#include "descriptor_set_provider.hpp"

#include "graphics_context.hpp"

namespace vk {

DescriptorSetProvider::DescriptorSetProvider(const GraphicsContext& context,
    handles::DescriptorPoolCreateInfo poolCreateInfo,
    uint32_t pivotPoolCount)
    : m_context(context)
    , m_pivotPoolCount(pivotPoolCount)
    , m_poolMaxSetCount(poolCreateInfo.maxSets())
    , m_poolFlags(poolCreateInfo.flags())
{
    m_poolSizes.resize(poolCreateInfo.poolSizeCount());
    std::copy(poolCreateInfo.pPoolSizes(),
        poolCreateInfo.pPoolSizes() + poolCreateInfo.poolSizeCount(),
        m_poolSizes.begin());

    for (size_t i = 0; i < pivotPoolCount; ++i)
    {
        m_poolList.emplace_back(context.device(), poolCreateInfo);
    }
}

DescriptorSetProvider::~DescriptorSetProvider() {}

std::shared_ptr<handles::DescriptorSet> DescriptorSetProvider::set(
    const handles::DescriptorSetLayout& layout)
{
    for (auto iter = m_poolList.begin(); iter != m_poolList.end();)
    {
        if (iter->isFull())
        {
            ++iter;
            continue;
        } 

        if (iter->isEmpty() && m_poolList.size() > m_pivotPoolCount)
        {
            iter = m_poolList.erase(iter);
			continue;
        }

        return iter->allocateSet(layout);
    }

    m_poolList.emplace_back(m_context.device(),
        handles::DescriptorPoolCreateInfo{}
            .flags(m_poolFlags)
            .pPoolSizes(m_poolSizes.data())
            .poolSizeCount(m_poolSizes.size())
            .maxSets(m_poolMaxSetCount));
    return m_poolList.back().allocateSet(layout);
}

std::shared_ptr<handles::DescriptorSet> DescriptorSetProvider::sets(
    std::span<const handles::DescriptorSetLayout> layout)
{
    ASSERT(false, "not implemented");
    return nullptr;
}


}    //  namespace vk
