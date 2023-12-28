#pragma once

#include <icomputer.hpp>

namespace ogl {

class GraphicsContext;

class Computer : public IComputer
{
public:
    Computer(const GraphicsContext& context, IComputer::CreateInfo createInfo);

    virtual ::OperationContext start(IComputeTarget& target) override;
    virtual void finish(::OperationContext& target) override;

private:
    const GraphicsContext& m_context;
};

}
