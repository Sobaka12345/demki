#pragma once

#include <ipipeline.hpp>
#include <types.hpp>

namespace ogl {

struct RenderContext
{
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    IPipeline* pipeline = nullptr;
};

}
