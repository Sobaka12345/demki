#pragma once

#include "graphics_context.hpp"
#include "specific_operation_target.hpp"
#include "specific_buffer.hpp"

#include "handles/fence.hpp"
#include "handles/command_buffer.hpp"

#include <istorage_buffer.hpp>

namespace renderer::vk {

class StorageBuffer
    : public SpecificBase<IStorageBuffer, ISpecificOperationTarget, ISpecificBuffer>
{
public:
    StorageBuffer(GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);
    ~StorageBuffer();

    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual void draw(renderer::OperationContext& context) const override;

    virtual void waitFor(OperationContext& context) override;
    virtual void populateWaitInfo(OperationContext& context) override;
    virtual uint32_t currentFrameIndex() const override;

    //  IShaderResource interface
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;

    virtual void write(const void* data, size_t size, size_t offset = 0) override;
    virtual const void* read(size_t size, size_t offset = 0) const override;

private:
    bool m_emitWait;
    uint64_t m_elementCount;

    std::vector<VkSemaphore> m_computeWaitSemaphores;

    VkCommandBuffer m_commandBuffer;
    VkFence m_computeInFlightFence;
    VkSemaphore m_computeFinishedSemaphore;
};

}    //  namespace renderer::vk
