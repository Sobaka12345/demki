#pragma once
#include "handle.hpp"
#include "creators.hpp"

#include <vulkan/vulkan.h>

namespace vk { namespace handles {

class Device;
class CommandPool;

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

    VkResult begin(VkCommandBufferBeginInfo beginInfo = commandBufferBeginInfo(nullptr)) const;
    VkResult end() const;
    VkResult reset(VkCommandBufferResetFlags flags = 0) const;
    void free() const;

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

    void copyBuffer(VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> regions) const;
    void copyBufferToImage(VkBuffer src,
        VkImage dst,
        VkImageLayout dstLayout,
        std::span<const VkBufferImageCopy> regions) const;
    void pipelineBarrier(VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkDependencyFlags dependencyFlags,
        std::span<const VkImageMemoryBarrier> imageMemoryBarriers = {},
        std::span<const VkMemoryBarrier> memoryBarriers = {},
        std::span<const VkBufferMemoryBarrier> bufferMemoryBarriers = {}) const;

private:
    const Device& m_device;
    const CommandPool& m_pool;
};

}}    //  namespace vk::handles
