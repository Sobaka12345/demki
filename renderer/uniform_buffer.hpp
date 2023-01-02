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
    UniformBuffer(const Device& device, uint32_t objectsCount)
        : Buffer(device,
            (m_dynamicAlignment = dynamicAlignment(device)) * objectsCount,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
        m_descriptorBufferInfo = create::descriptorBufferInfo(buffer(), 0, dynamicAlignment());
    }

    uint32_t dynamicAlignment() const { return m_dynamicAlignment; }
    const VkDescriptorBufferInfo* descriptor() const { return &m_descriptorBufferInfo; }

private:
    uint32_t dynamicAlignment(const Device& device) const
    {
        const uint32_t minAlignment =
            device.physicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
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
