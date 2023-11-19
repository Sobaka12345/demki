#include "command_buffer.hpp"

#include "command_pool.hpp"
#include "descriptor_set.hpp"
#include "graphics_pipeline.hpp"
#include "pipeline_layout.hpp"

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
    const auto allocateInfo =
        CommandBufferAllocateInfo{}.commandPool(pool).level(level).commandBufferCount(1);
    ASSERT(create(vkAllocateCommandBuffers, device, &allocateInfo) == VK_SUCCESS,
        "failed to create CommandBuffer");
}

VkResult CommandBuffer::begin(CommandBufferBeginInfo beginInfo) const
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

void CommandBuffer::draw(uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) const
{
    vkCmdDraw(handle(), vertexCount, instanceCount, firstVertex, firstInstance);
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

void CommandBuffer::bindPipeline(const Pipeline& pipeline, VkPipelineBindPoint bindPoint) const
{
    vkCmdBindPipeline(handle(), bindPoint, pipeline);
}

void CommandBuffer::bindDescriptorSet(const PipelineLayout& layout,
    uint32_t firstSet,
    const DescriptorSet& set,
    std::span<const uint32_t> dynamicOffsets,
    VkPipelineBindPoint bindPoint) const
{
    vkCmdBindDescriptorSets(handle(), bindPoint, layout, firstSet, 1, set.handlePtr(),
        dynamicOffsets.size(), dynamicOffsets.data());
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
    std::span<const ImageMemoryBarrier> imageMemoryBarriers,
    std::span<const VkMemoryBarrier> memoryBarriers,
    std::span<const VkBufferMemoryBarrier> bufferMemoryBarriers) const
{
    vkCmdPipelineBarrier(handle(), srcStageMask, dstStageMask, dependencyFlags,
        static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
        static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(),
        static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
}

void CommandBuffer::blitImage(VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    std::span<const ImageBlit> regions,
    VkFilter filter) const
{
    vkCmdBlitImage(handle(), srcImage, srcImageLayout, dstImage, dstImageLayout, regions.size(),
        regions.data(), filter);
}

void CommandBuffer::setViewports(
    uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) const
{
    vkCmdSetViewport(handle(), 0, 1, pViewports);
}

void CommandBuffer::setViewport(VkViewport viewport) const
{
    setViewports(0, 1, &viewport);
}

void CommandBuffer::setScissors(
    uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) const
{
    vkCmdSetScissor(handle(), 0, 1, pScissors);
}

void CommandBuffer::setScissor(VkRect2D scissor) const
{
    setScissors(0, 1, &scissor);
}

void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const
{
    vkCmdDispatch(handle(), groupCountX, groupCountY, groupCountZ);
}


}}    //  namespace vk::handles
