#pragma once

#include "handles/semaphore.hpp"
#include <ishader_interface.hpp>

#include <types.hpp>

#include <unordered_map>
#include <vector>

namespace renderer {

struct OperationContext;

class IPipeline;
class IOperationTarget;

namespace vk {

class ISpecificOperationTarget;
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

    void submit(renderer::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    std::vector<VkSemaphore> waitSemaphores;
    handles::Framebuffer* framebuffer = nullptr;
    handles::CommandBuffer* commandBuffer = nullptr;
    ISpecificOperationTarget* specificTarget = nullptr;
    IOperationTarget* mainTarget = nullptr;

    ComputePipeline* computePipeline = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
    Renderer* renderer = nullptr;
    Computer* computer = nullptr;
    handles::RenderPass* renderPass = nullptr;
};

}    //  namespace vk
}    //  namespace renderer
