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

    handles::Semaphore::Vector<> waitSemaphores;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFormat depthFormat = VK_FORMAT_UNDEFINED;
    VkFormat imageFormat = VK_FORMAT_UNDEFINED;

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
