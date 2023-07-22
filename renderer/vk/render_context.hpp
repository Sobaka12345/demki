#pragma once

#include "pipeline.hpp"
#include "uniform_handle.hpp"

#include <uniform.hpp>
#include <types.hpp>

#include <unordered_map>

namespace vk {

namespace handles {

class CommandBuffer;
class Framebuffer;
class PipelineLayout;
class RenderPass;

}

struct RenderContext
{
    ~RenderContext();
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    Pipeline* pipeline = nullptr;
    handles::Framebuffer* framebuffer = nullptr;
    handles::CommandBuffer* commandBuffer = nullptr;
    handles::RenderPass* const renderPass = nullptr;
};

}    //  namespace vk
