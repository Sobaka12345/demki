#include "storage_buffer.hpp"

#include "handles/buffer.hpp"
#include "handles/command_pool.hpp"
#include "handles/device.hpp"
#include "handles/fence.hpp"
#include "handles/semaphore.hpp"
#include "handles/queue.hpp"

#include "compute_pipeline.hpp"
#include "resource_manager.hpp"

namespace vk {

StorageBuffer::StorageBuffer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_elementCount(createInfo.size / createInfo.elementLayoutSize)
    , m_commandBuffer(std::make_unique<handles::CommandBuffer>(
          context.device().commandPool(handles::GRAPHICS_COMPUTE).lock()->allocateBuffer()))
{
    m_handle =
        context.resourcesSpecific().fetchHandleSpecific(ShaderBlockType::STORAGE, createInfo.size);

    m_handle->write(createInfo.initialData, createInfo.size);

    m_computeFinishedSemaphore =
        std::make_unique<handles::Semaphore>(context.device(), handles::SemaphoreCreateInfo{});

    m_computeInFlightFence = std::make_unique<handles::Fence>(context.device(),
        handles::FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT));
}

void StorageBuffer::accept(ComputerInfoVisitor& visitor) const
{
    visitor.populateComputerInfo(*this);
}

bool StorageBuffer::prepare(::OperationContext& context)
{
    vkWaitForFences(
        m_context.device(), 1, m_computeInFlightFence->handlePtr(), VK_TRUE, UINT64_MAX);

    vkResetFences(m_context.device(), 1, m_computeInFlightFence->handlePtr());

    auto& specContext = get(context);
    specContext.commandBuffer = m_commandBuffer.get();
    specContext.computeTarget = this;

    m_commandBuffer->reset();
    return m_commandBuffer->begin() == VK_SUCCESS;
}

void StorageBuffer::compute(::OperationContext& context)
{
    auto& specContext = get(context);
    auto [x, y, z] = specContext.computePipeline->computeDimensions();

    //  move element count to some more logically suitable place?
    m_commandBuffer->dispatch(m_elementCount / x, y, z);

    ASSERT(m_commandBuffer->end() == VK_SUCCESS, "failed to end command buffer");

    const auto submitInfo =
        handles::SubmitInfo{}
            .commandBufferCount(1)
            .pCommandBuffers(m_commandBuffer->handlePtr())
            .signalSemaphoreCount(1)
            .pSignalSemaphores(m_computeFinishedSemaphore->handlePtr());

    ASSERT(m_context.device()
                .queue(handles::GRAPHICS_COMPUTE)
                .lock()
                ->submit(1, &submitInfo, *m_computeInFlightFence) == VK_SUCCESS,
        "failed to submit compute command buffer!");
}

void StorageBuffer::bind(::OperationContext& context) const
{
    VkBuffer buf = m_handle->currentDescriptor()->descriptorBufferInfo.buffer();
    VkDeviceSize offset = m_handle->currentDescriptor()->descriptorBufferInfo.offset();
    get(context).commandBuffer->bindVertexBuffer(0, 1, &buf, &offset);
}

std::weak_ptr<IShaderInterfaceHandle> StorageBuffer::handle() const
{
    return m_handle;
}

}    //  namespace vk
