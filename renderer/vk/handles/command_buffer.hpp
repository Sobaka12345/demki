#pragma once

#include "image.hpp"

#include "vk/utils.hpp"

#include <vulkan/vulkan.h>

#include <span>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(CommandBufferAllocateInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO)
    VKSTRUCT_PROPERTY(VkStructureType, sType)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandPool, commandPool)
    VKSTRUCT_PROPERTY(VkCommandBufferLevel, level)
    VKSTRUCT_PROPERTY(uint32_t, commandBufferCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(CommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandBufferUsageFlags, flags)
    VKSTRUCT_PROPERTY(const VkCommandBufferInheritanceInfo*, pInheritanceInfo)
END_DECLARE_VKSTRUCT()

class Device;
class CommandPool;
class DescriptorSet;
class Pipeline;
class PipelineLayout;

class CommandBuffer : public Handle<VkCommandBuffer>
{
public:
    CommandBuffer(const CommandBuffer& other) = delete;
    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer(const Device& device, const CommandPool& pool, VkHandleType* handlePtr = nullptr);
    CommandBuffer(const Device& device,
        const CommandPool& pool,
        VkCommandBufferLevel level,
        VkHandleType* handlePtr = nullptr);
    ~CommandBuffer();

    VkResult begin(
        CommandBufferBeginInfo beginInfo = CommandBufferBeginInfo{}.pInheritanceInfo(
            nullptr)) const;
    VkResult end() const;
    VkResult reset(VkCommandBufferResetFlags flags = 0) const;
    void free() const;

    void draw(uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance) const;
    void drawIndexed(uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        uint32_t vertexOffset,
        uint32_t firstInstance) const;
    void bindVertexBuffer(uint32_t firstBinding,
        uint32_t bindingCount,
        const VkBuffer* pBuffers,
        const VkDeviceSize* pOffsets) const;
    void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
    void bindPipeline(const Pipeline& pipeline,
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS) const;
    void bindDescriptorSet(const PipelineLayout& layout,
        uint32_t firstSet,
        const DescriptorSet& set,
        std::span<const uint32_t> dynamicOffsets,
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS) const;

    void copyBuffer(VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> regions) const;
    void copyBufferToImage(VkBuffer src,
        VkImage dst,
        VkImageLayout dstLayout,
        std::span<const VkBufferImageCopy> regions) const;
    void pipelineBarrier(VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkDependencyFlags dependencyFlags,
        std::span<const ImageMemoryBarrier> imageMemoryBarriers = {},
        std::span<const VkMemoryBarrier> memoryBarriers = {},
        std::span<const VkBufferMemoryBarrier> bufferMemoryBarriers = {}) const;
    void blitImage(VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        std::span<const ImageBlit> regions,
        VkFilter filter) const;

    void setViewports(
        uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) const;
    void setViewport(VkViewport viewport) const;

    void setScissors(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) const;
    void setScissor(VkRect2D scissor) const;

    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const;

private:
    const Device& m_device;
    const CommandPool& m_pool;
};

}}    //  namespace vk::handles
