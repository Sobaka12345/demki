#pragma once

#include <ipipeline.hpp>
#include <types.hpp>

namespace renderer {

struct OperationContext;
struct IOperationTarget;

namespace ogl {

class Computer;
class Renderer;
class PipelineBindContext;

class ISpecificOperationTarget;

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

    void draw(uint32_t count, uint32_t instances){};
    void submit(renderer::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    IPipeline* pipeline();
    IOperationTarget* operationTarget();

    Renderer* renderer = nullptr;
    Computer* computer = nullptr;

    ISpecificOperationTarget* specificTarget = nullptr;
    IOperationTarget* mainTarget = nullptr;

    PipelineBindContext* pipelineBindContext = nullptr;
    ComputePipeline* computePipeline = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
};

}    //  namespace ogl

}    //  namespace renderer
