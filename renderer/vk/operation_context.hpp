#pragma once

#include "handles/command_buffer.hpp"
#include "handles/framebuffer.hpp"
#include "handles/render_pass.hpp"
#include "handles/semaphore.hpp"

#include <types.hpp>

namespace renderer {

struct OperationContext;

class IPipeline;
class IOperationTarget;

namespace vk {

class ISpecificOperationTarget;
class Computer;
class Renderer;
class ISpecificPipeline;

class PipelineBindContext;
class ComputePipeline;
class GraphicsPipeline;

struct OperationContext
{
    OperationContext() = default;
    OperationContext(Computer* computer);
    OperationContext(Renderer* renderer);
    OperationContext(OperationContext&& other);
    OperationContext(const OperationContext& other) = delete;
    ~OperationContext();

    IPipeline* pipeline();
    ISpecificPipeline* specificPipeline();
    IOperationTarget* operationTarget();

    void submit(::renderer::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    handles::SemaphoreContainer::Vector<> waitSemaphores;
    handles::Framebuffer framebuffer = VK_NULL_HANDLE;
    handles::CommandBuffer commandBuffer = VK_NULL_HANDLE;
    handles::RenderPass renderPass = VK_NULL_HANDLE;

    ISpecificOperationTarget* specificTarget = nullptr;
    IOperationTarget* mainTarget = nullptr;

    ComputePipeline* computePipeline = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
    PipelineBindContext* pipelineBindContext = nullptr;
    Renderer* renderer = nullptr;
    Computer* computer = nullptr;
};

}    //  namespace vk
}    //  namespace renderer
