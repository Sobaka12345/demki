#include "storage_buffer.hpp"

#include "handles/buffer.hpp"
#include "handles/command_pool.hpp"
#include "handles/device.hpp"
#include "handles/fence.hpp"
#include "handles/semaphore.hpp"
#include "handles/queue.hpp"

#include "compute_pipeline.hpp"

#include <cstring>

namespace renderer::vk {

StorageBuffer::StorageBuffer(GraphicsContext& context, CreateInfo createInfo)
    : SpecificBase<IStorageBuffer, ISpecificOperationTarget, ISpecificBuffer>(context)
{
    allocateBuffer(createInfo.size(),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

StorageBuffer::~StorageBuffer()
{
    destroy();
}

bool StorageBuffer::prepare(renderer::OperationContext& context)
{
    //  vkWaitForFences(m_context.device(), 1, m_computeInFlightFence->handlePtr(), VK_TRUE,
    //      UINT64_MAX);

    //  vkResetFences(m_context.device(), 1, m_computeInFlightFence->handlePtr());

    auto& specContext = get(context);
    specContext.commandBuffer = m_commandBuffer;
    specContext.specificTarget = this;

    vkResetCommandBuffer(m_commandBuffer, 0);
    const auto commandBufferBeginInfo = CommandBufferBeginInfo{}.pInheritanceInfo(nullptr);
    return vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS;
}

void StorageBuffer::present(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    auto [x, y, z] = specContext.computePipeline->computeDimensions();

    //  move element count to some more logically suitable place?
    vkCmdDispatch(m_commandBuffer, m_elementCount / x, y, z);

    ASSERT(vkEndCommandBuffer(m_commandBuffer) == VK_SUCCESS, "failed to end command buffer");

    auto submitInfo = SubmitInfo{}.commandBufferCount(1).pCommandBuffers(&m_commandBuffer);

    if (!m_computeWaitSemaphores.empty())
    {
        submitInfo.waitSemaphoreCount(m_computeWaitSemaphores.size())
            .pWaitSemaphores(m_computeWaitSemaphores.data());
        m_computeWaitSemaphores.clear();
    }

    if (m_emitWait)
    {
        submitInfo.signalSemaphoreCount(1).pSignalSemaphores(&m_computeFinishedSemaphore);
        m_emitWait = false;
    }

    ASSERT(vkQueueSubmit(m_context.queue(QueueFamilyType::GRAPHICS_COMPUTE),
               1,
               &submitInfo,
               m_computeInFlightFence) == VK_SUCCESS,
        "failed to submit compute command buffer!");
}

void StorageBuffer::draw(renderer::OperationContext& context) const {}

void StorageBuffer::waitFor(OperationContext& context)
{
    std::copy(context.waitSemaphores.begin(),
        context.waitSemaphores.end(),
        std::back_inserter(m_computeWaitSemaphores));
}

void StorageBuffer::populateWaitInfo(OperationContext& context) {}

uint32_t StorageBuffer::currentFrameIndex() const
{
    return 0;
}

void StorageBuffer::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    //  NOTHING TO DO
}

void StorageBuffer::write(const void* data, size_t size, size_t offset)
{
    std::memcpy(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(m_data) + offset), data,
        static_cast<size_t>(size));
}

const void* StorageBuffer::read(size_t size, size_t offset) const
{
    return (static_cast<char*>(m_data)) + offset;
}

}    //  namespace renderer::vk
