#pragma once

#include "graphics_context.hpp"
#include "specific_operation_target.hpp"
#include "specific_shader_resource.hpp"

#include <istorage_buffer.hpp>

namespace renderer::vk {

namespace handles {
class Semaphore;
class Fence;
}

class StorageBuffer
    : public SpecificBase<IStorageBuffer, ISpecificOperationTarget, ISpecificShaderResource>
{
public:
    StorageBuffer(GraphicsContext& context, IStorageBuffer::CreateInfo createInfo);

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual void draw(renderer::OperationContext& context) const override;

    virtual void waitFor(OperationContext& context) override;
    virtual void populateWaitInfo(OperationContext& context) override;
    virtual uint32_t descriptorsRequired() const override;

    //  IShaderResource interface
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;
    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) override;

private:
    const GraphicsContext& m_context;

    bool m_emitWait;
    uint64_t m_elementCount;

    std::vector<VkSemaphore> m_computeWaitSemaphores;

    std::shared_ptr<ShaderInterfaceHandle> m_handle;

    std::unique_ptr<handles::CommandBuffer> m_commandBuffer;
    std::unique_ptr<handles::Fence> m_computeInFlightFence;
    std::unique_ptr<handles::Semaphore> m_computeFinishedSemaphore;
};

}    //  namespace renderer::vk
