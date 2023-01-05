#pragma once

#include "descriptor_set.hpp"
#include "uniform_buffer.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>

namespace vk {

template <typename LayoutType>
class UBOValue
{
    constexpr static VkDeviceSize s_layoutSize = sizeof(LayoutType);

public:
    UBOValue(const DescriptorSet* descriptorSet, std::shared_ptr<UBOHandler> handler)
        : m_descriptorSet(descriptorSet)
        , m_uboHandler(handler)
    {}

    void set(LayoutType value)
    {
        m_value = value;
        if (!m_uboHandler->memory.expired())
        {
            m_uboHandler->memory.lock()->mapped->write(&m_value, s_layoutSize, m_uboHandler->offset);
        }
    }

    LayoutType get() const
    {
        return m_value;
    }

    void sync()
    {
        if (!m_uboHandler->memory.expired())
        {
            m_uboHandler->memory.lock()->mapped->sync(s_layoutSize, m_uboHandler->offset);
        }
    }

    // TO DO: REMOVE THIS CRINGE
    void bind(VkCommandBuffer commandBuffer)
    {
        m_descriptorSet->bind(commandBuffer, m_uboHandler->offset);
    }

private:
    LayoutType m_value;
    const DescriptorSet* m_descriptorSet;
    std::shared_ptr<UBOHandler> m_uboHandler;
};

}
