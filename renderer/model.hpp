#pragma once

#include "vk/buffer.hpp"
#include "vk/command_buffer.hpp"
#include "vk/resource_manager.hpp"

#include <span>
#include <memory>
#include <vector>

class Model : public vk::Resource
{
public:
    friend class ResourceManager;
    struct Descriptor : public vk::Resource::Descriptor
    {
        vk::Buffer::Descriptor vertices;
        vk::Buffer::Descriptor indices;
    };

public:
    Model(Descriptor descriptor)
        : m_descriptor(std::move(descriptor))
    {
    }

    virtual void draw(const vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.drawIndexed(m_descriptor.indices.objectCount(), 1, 0, 0, 0);
    }

    void bind(const vk::CommandBuffer& commandBuffer)
    {
        VkDeviceSize offsets[] = {m_descriptor.vertices.offset};
        commandBuffer.bindVertexBuffer(0, 1, m_descriptor.vertices.buffer.handlePtr(), offsets);
        commandBuffer.bindIndexBuffer(m_descriptor.indices.buffer, m_descriptor.indices.offset, VK_INDEX_TYPE_UINT32);
    }

private:
    Descriptor m_descriptor;
};
