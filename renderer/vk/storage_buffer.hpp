#pragma once

#include "graphics_context.hpp"
#include "compute_target.hpp"

#include <istorage_buffer.hpp>

namespace vk {

namespace handles {
class Semaphore;
class Fence;
}

class StorageBuffer
    : public IStorageBuffer
    , public ComputeTarget
{
public:
    StorageBuffer(const GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(::OperationContext& context) override;
    virtual void compute(::OperationContext& context) override;

    virtual void bind(::OperationContext& context) const override;
    virtual void draw(::OperationContext& context) const override;
    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const override;

    virtual void waitFor(OperationContext& context) override;
    virtual void populateWaitInfo(OperationContext& context) override;
    virtual uint32_t descriptorsRequired() const override;

private:
    const GraphicsContext& m_context;

    bool m_emitWait;
    uint64_t m_elementCount;

    std::vector<VkSemaphore> m_computeWaitSemaphores;

    std::unique_ptr<handles::CommandBuffer> m_commandBuffer;
    std::unique_ptr<handles::Fence> m_computeInFlightFence;
    std::unique_ptr<handles::Semaphore> m_computeFinishedSemaphore;
    std::shared_ptr<ShaderInterfaceHandle> m_handle;
};

}    //  namespace vk
