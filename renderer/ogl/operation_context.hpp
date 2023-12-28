#pragma once

#include <ipipeline.hpp>
#include <types.hpp>

struct OperationContext;
struct IOperationTarget;

namespace ogl {

class Computer;
class Renderer;
class ComputeTarget;
class RenderTarget;

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

    void submit(::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;
    IPipeline* pipeline();

    IPipeline* gpipeline = nullptr;

    Renderer* renderer = nullptr;
    Computer* computer = nullptr;
    ComputeTarget* computeTarget = nullptr;
    RenderTarget* renderTarget = nullptr;
    ComputePipeline* computePipeline = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
};

}    //  namespace ogl
