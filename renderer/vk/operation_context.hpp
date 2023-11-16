#pragma once

#include "pipeline.hpp"

#include <shader_interface.hpp>
#include <types.hpp>

#include <unordered_map>
#include <vector>

struct OperationContext;

namespace vk {

class Computer;
class Renderer;

class ComputeTarget;
class RenderTarget;

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

    void submit(::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    Pipeline* pipeline = nullptr;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkSemaphore> finishSemaphores;
    handles::Framebuffer* framebuffer = nullptr;
    handles::CommandBuffer* commandBuffer = nullptr;
    ComputeTarget* computeTarget = nullptr;
    RenderTarget* renderTarget = nullptr;

    Renderer* renderer = nullptr;
    Computer* computer = nullptr;
    handles::RenderPass* renderPass = nullptr;
};

}    //  namespace vk
