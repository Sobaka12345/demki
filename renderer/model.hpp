#pragma once

#include "buffer.hpp"
#include "command_buffer.hpp"

#include <span>
#include <memory>
#include <vector>

class Model : std::enable_shared_from_this<Model>
{
public:
    static std::shared_ptr<Model> create(vk::VertexBuffer* vertices, vk::IndexBuffer* indices)
    {
        return std::shared_ptr<Model>(new Model(vertices, indices));
    }

    std::shared_ptr<Model> ptr()
    {
        return shared_from_this();
    }

    virtual void draw(const vk::CommandBuffer& commandBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, m_indexBuffer->indexCount(), 1, 0, 0, 0);
    }

    void bind(const vk::CommandBuffer& commandBuffer)
    {
        VkBuffer vertexBuffers[] = {m_vertexBuffer->handle()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->handle(), 0, VK_INDEX_TYPE_UINT16);
    }

private:
    Model(vk::VertexBuffer* vertices, vk::IndexBuffer* indices)
        : m_indexBuffer(indices)
        , m_vertexBuffer(vertices)
    {
    }

private:
    std::unique_ptr<vk::IndexBuffer> m_indexBuffer;
    std::unique_ptr<vk::VertexBuffer> m_vertexBuffer;
};
