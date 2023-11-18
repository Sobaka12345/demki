#pragma once

#include "graphics_context.hpp"

#include <istorage_buffer.hpp>

namespace vk {

namespace handles {
class Semaphore;
class Fence;
}

class StorageBuffer : public IStorageBuffer
{
public:
    StorageBuffer(const GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(::OperationContext& context) override;
    virtual void compute(::OperationContext& context) override;

    virtual void bind(::OperationContext& context) const override;
    virtual std::weak_ptr<IShaderInterfaceHandle> handle() const override;

private:
    const GraphicsContext& m_context;

    uint64_t m_elementCount;

    std::unique_ptr<handles::CommandBuffer> m_commandBuffer;
    std::unique_ptr<handles::Fence> m_computeInFlightFence;
    std::unique_ptr<handles::Semaphore> m_computeFinishedSemaphore;
    std::shared_ptr<ShaderInterfaceHandle> m_handle;
};

}    //  namespace vk
