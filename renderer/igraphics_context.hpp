#pragma once

#include <icomputer.hpp>
#include <icompute_pipeline.hpp>
#include <igraphics_pipeline.hpp>
#include <irenderer.hpp>
#include <iswapchain.hpp>
#include <istorage_buffer.hpp>
#include <iresource_manager.hpp>
#include <types.hpp>

class IGraphicsContext
{
public:
    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<ISwapchain> createSwapchain(
        ISwapchain::CreateInfo createInfo) const = 0;

    virtual IResourceManager& resources() const = 0;

    virtual Multisampling maxSampleCount() const = 0;

    virtual void waitIdle() = 0;

    virtual ~IGraphicsContext() {}
};
