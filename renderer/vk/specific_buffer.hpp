#pragma once

#include "../utils.hpp"

#include "handles/buffer.hpp"
#include "handles/memory.hpp"

#include <ibuffer.hpp>

namespace renderer { namespace vk {

class GraphicsContext;

struct ISpecificBuffer
{
protected:
    ISpecificBuffer(GraphicsContext& context)
        : m_context(context)
    {}

    ~ISpecificBuffer() {};

    virtual IBuffer* toBase() = 0;

    void allocateBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void destroy();

    void setActiveRange(size_t size, size_t offset);

protected:
    GraphicsContext& m_context;

    void* m_data = nullptr;
    size_t m_alignment = 0;
    size_t m_size = 0;
    size_t m_dynamicSize = m_size;
    size_t m_dynamicOffset = 0;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
};

template <typename IBase>
using SpecificBuffer = SpecificBase<IBase, ISpecificBuffer>;

}}    //  namespace renderer::vk
