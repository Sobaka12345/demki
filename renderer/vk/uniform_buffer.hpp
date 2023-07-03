#pragma once

#include "handles/buffer.hpp"
#include "handles/creators.hpp"

#include <glm/mat4x4.hpp>

#include <map>

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

struct UBOHandler
{
    uint32_t offset;
    std::weak_ptr<handles::Memory> memory;
};

struct IUBOProvider
{
    virtual std::shared_ptr<UBOHandler> tryGetUBOHandler() = 0;
};

template <typename Layout>
class UniformBuffer
    : public handles::Buffer
    , public IUBOProvider
{
public:
    UniformBuffer(const handles::Device& device, uint32_t objectsCount)
        : Buffer(device,
              handles::BufferCreateInfo()
                  .size((m_dynamicAlignment = dynamicAlignment(device)) * objectsCount)
                  .usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                  .sharingMode(VK_SHARING_MODE_EXCLUSIVE))
        , m_objectsCount(objectsCount)
    {
        m_uboHandlers.resize(objectsCount);
        m_descriptorBufferInfo = handles::descriptorBufferInfo(handle(), 0, dynamicAlignment());
    }

    virtual std::shared_ptr<UBOHandler> tryGetUBOHandler() override
    {
        uint32_t index = 0;
        for (; index < m_uboHandlers.size(); ++index)
        {
            if (m_uboHandlers[index].expired())
            {
                break;
            }
        }
        if (index == m_uboHandlers.size())
        {
            return nullptr;
        }

        auto result = std::shared_ptr<UBOHandler>(new UBOHandler);
        result->offset = index * dynamicAlignment();
        result->memory = memory();
        m_uboHandlers[index] = result;

        return result;
    }

    uint32_t objectsCount() const { return m_objectsCount; }

    uint32_t dynamicAlignment() const { return m_dynamicAlignment; }

    VkDescriptorBufferInfo descriptorBufferInfo() const { return m_descriptorBufferInfo; }

private:
    uint32_t dynamicAlignment(const handles::Device& device) const
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
    std::vector<std::weak_ptr<UBOHandler>> m_uboHandlers;
    uint32_t m_objectsCount;
    uint32_t m_dynamicAlignment;
    VkDescriptorBufferInfo m_descriptorBufferInfo;
};

}    //  namespace vk
