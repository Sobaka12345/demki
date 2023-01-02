#pragma once

#include "buffer.hpp"

#include <span>
#include <memory>
#include <vector>

//template <typename VertexType, typename IndexType = uint16_t>
class Model
{
    Model(vk::Buffer vertices, vk::Buffer indices)
        : m_indexBuffer(std::make_unique<vk::Buffer>(std::move(indices)))
        , m_vertexBuffer(std::make_unique<vk::Buffer>(std::move(vertices)))
    {
    }

private:
    std::unique_ptr<vk::Buffer> m_indexBuffer;
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
};
