#pragma once

#include "buffer.hpp"
#include "creators.hpp"

#include <glm/mat4x4.hpp>

namespace vk {

struct UBOModel
{
    glm::mat4 model;
};

struct UBOViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

template<typename Layout>
class UniformBuffer: public Buffer
{
public:
    UniformBuffer(): Buffer() {}

    UniformBuffer(VkPhysicalDeviceLimits limits, VkDevice device, uint32_t objectsCount)
        : Buffer(device, (m_dynamicAlignment = dynamicAlignment(limits)) * objectsCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
        m_descriptorBufferInfo = create::descriptorBufferInfo(buffer(), 0, m_dynamicAlignment);
    }

    uint32_t dynamicAlignment() const { return m_dynamicAlignment; }
    const VkDescriptorBufferInfo* descriptor() const { return &m_descriptorBufferInfo; }

private:
    uint32_t dynamicAlignment(VkPhysicalDeviceLimits limits) const
    {
        const uint32_t minAlignment = limits.minUniformBufferOffsetAlignment;
        uint32_t result = sizeof(Layout);
        if (minAlignment > 0)
        {
            result = (result + minAlignment - 1) & ~(minAlignment - 1);
        }

        return result;
    }

private:
    uint32_t m_dynamicAlignment;
    VkDescriptorBufferInfo m_descriptorBufferInfo;
};

}
