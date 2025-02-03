#pragma once

#include <icomputer.hpp>

namespace renderer::ogl {

class GraphicsContext;

class Computer : public IComputer
{
public:
    Computer(const GraphicsContext& context, IComputer::CreateInfo createInfo);

    virtual renderer::OperationContext start(IComputeTarget& target) override;
    virtual void finish(renderer::OperationContext& context) override;

private:
    const GraphicsContext& m_context;
};

}
