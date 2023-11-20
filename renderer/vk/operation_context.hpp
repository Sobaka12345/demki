#pragma once

#include "handles/semaphore.hpp"

#include <shader_interface.hpp>
#include <types.hpp>

#include <unordered_map>
#include <vector>

struct OperationContext;

class IPipeline;

namespace vk {

class IOperationTarget;
class ComputeTarget;
class RenderTarget;
class Computer;
class Renderer;

class ComputePipeline;
class GraphicsPipeline;

namespace handles {

class CommandBuffer;
class Framebuffer;
class PipelineLayout;
class RenderPass;
class Semaphore;

}

struct OperationContext
{
    OperationContext() = default;
    OperationContext(Computer* computer);
    OperationContext(Renderer* renderer);
    OperationContext(OperationContext&& other);
    OperationContext(const OperationContext& other) = delete;
    ~OperationContext();

    IPipeline* pipeline();

    IOperationTarget* operationTarget();

    void submit(::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    std::vector<VkSemaphore> waitSemaphores;
    handles::Framebuffer* framebuffer = nullptr;
    handles::CommandBuffer* commandBuffer = nullptr;
    ComputeTarget* computeTarget = nullptr;
    RenderTarget* renderTarget = nullptr;

    ComputePipeline* computePipeline = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
    Renderer* renderer = nullptr;
    Computer* computer = nullptr;
    handles::RenderPass* renderPass = nullptr;
};

}    //  namespace vk
