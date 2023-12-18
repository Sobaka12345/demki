#pragma once

#include "handles/descriptor_pool.hpp"

#include <list>
#include <memory>

namespace vk {

class GraphicsContext;

class DescriptorSetProvider
{
public:
    DescriptorSetProvider(const GraphicsContext& context,
        handles::DescriptorPoolCreateInfo poolCreateInfo,
        uint32_t pivotPoolCount = 1);
    ~DescriptorSetProvider();

    std::shared_ptr<handles::DescriptorSet> set(const handles::DescriptorSetLayout& layout);
    std::shared_ptr<handles::DescriptorSet> sets(
        std::span<const handles::DescriptorSetLayout> layout);

private:
    const GraphicsContext& m_context;
    const uint32_t m_pivotPoolCount;
    const uint32_t m_poolMaxSetCount;
    const VkDescriptorPoolCreateFlags m_poolFlags;
    std::vector<handles::DescriptorPoolSize> m_poolSizes;

    std::list<handles::DescriptorPool> m_poolList;
};

}    //  namespace vk
