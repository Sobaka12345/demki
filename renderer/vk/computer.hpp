#pragma once

#include <icomputer.hpp>

#include "graphics_context.hpp"

#include <map>

namespace vk {

class Computer : public IComputer
{
public:
    Computer(const GraphicsContext& context, IComputer::CreateInfo createInfo);

    virtual ::OperationContext start(IComputeTarget& target) override;
    virtual void finish(::OperationContext& context) override;


private:
    const GraphicsContext& m_context;
};

}    //  namespace vk
