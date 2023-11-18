#pragma once

#include <ipipeline.hpp>
#include <types.hpp>

struct OperationContext;

namespace ogl {

struct OperationContext
{
    void submit(::OperationContext& context);
    void waitForOperation(OperationContext& other);
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;
    IPipeline* pipeline();

    IPipeline* gpipeline = nullptr;
};

}
