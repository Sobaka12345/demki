#include "command_buffer.hpp"
#include "command_pool.hpp"
#include "creators.hpp"

namespace vk { namespace handles {

CommandBuffer::CommandBuffer(const Device& device, const CommandPool& pool, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_pool(pool)
{
    setOwner(false);
}

CommandBuffer::~CommandBuffer()
{
    destroy(vkFreeCommandBuffers, m_device, m_pool, 1, handlePtr());
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
    , m_pool(other.m_pool)
{}

CommandBuffer::CommandBuffer(const Device& device,
    const CommandPool& pool,
    VkCommandBufferLevel level,
    VkHandleType* handlePtr)
    : Handle(handlePtr)
    , m_device(device)
    , m_pool(pool)
{
    const auto allocateInfo = commandBufferAllocateInfo(pool, level, 1);
    ASSERT(create(vkAllocateCommandBuffers, device, &allocateInfo) == VK_SUCCESS,
        "failed to create CommandBuffer");
}

VkResult CommandBuffer::begin(VkCommandBufferBeginInfo beginInfo) const
{
    return vkBeginCommandBuffer(handle(), &beginInfo);
}

VkResult CommandBuffer::end() const
{
    return vkEndCommandBuffer(handle());
}

VkResult CommandBuffer::reset(VkCommandBufferResetFlags flags) const
{
    return vkResetCommandBuffer(handle(), flags);
}

void CommandBuffer::free() const
{
    vkFreeCommandBuffers(m_device, m_pool, 1, handlePtr());
}

void CommandBuffer::drawIndexed(uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    uint32_t vertexOffset,
    uint32_t firstInstance) const
{
    vkCmdDrawIndexed(handle(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::bindVertexBuffer(uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets) const
{
    vkCmdBindVertexBuffers(handle(), firstBinding, bindingCount, pBuffers, pOffsets);
}

void CommandBuffer::bindIndexBuffer(
    VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const
{
    vkCmdBindIndexBuffer(handle(), buffer, offset, indexType);
}

void CommandBuffer::copyBuffer(
    VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> regions) const
{
    vkCmdCopyBuffer(handle(), src, dst, static_cast<uint32_t>(regions.size()), regions.data());
}

void CommandBuffer::copyBufferToImage(VkBuffer src,
    VkImage dst,
    VkImageLayout dstLayout,
    std::span<const VkBufferImageCopy> regions) const
{
    vkCmdCopyBufferToImage(handle(), src, dst, dstLayout, static_cast<uint32_t>(regions.size()),
        regions.data());
}

void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    std::span<const VkImageMemoryBarrier> imageMemoryBarriers,
    std::span<const VkMemoryBarrier> memoryBarriers,
    std::span<const VkBufferMemoryBarrier> bufferMemoryBarriers) const
{
    vkCmdPipelineBarrier(handle(), srcStageMask, dstStageMask, dependencyFlags,
        static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
        static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(),
        static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
}

}}    //  namespace vk::handles
