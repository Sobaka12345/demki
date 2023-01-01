#pragma once

#include "buffer.hpp"

#include <span>
#include <memory>
#include <vector>

template <typename VertexType>
class Model
{
    Model(std::span<VertexType> vertices, std::span<uint16_t> indices)
    {

    }

    vk::Buffer buffer() const
    {

    }

private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;
};
