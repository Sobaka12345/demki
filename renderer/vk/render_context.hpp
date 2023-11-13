#pragma once

#include "pipeline.hpp"

#include <shader_interface.hpp>
#include <types.hpp>

#include <unordered_map>

namespace vk {

class RenderTarget;

namespace handles {

class CommandBuffer;
class Framebuffer;
class PipelineLayout;
class RenderPass;
class Renderer;

}

struct RenderContext
{
    ~RenderContext();
    void setScissors(Scissors scissors) const;
    void setViewport(Viewport viewport) const;

    Pipeline* pipeline = nullptr;
    handles::Framebuffer* framebuffer = nullptr;
    handles::CommandBuffer* commandBuffer = nullptr;
    RenderTarget* renderTarget = nullptr;

    Renderer* const renderer = nullptr;
    handles::RenderPass* const renderPass = nullptr;
};

}    //  namespace vk
