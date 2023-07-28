#pragma once

#include <ipipeline.hpp>
#include <irenderer.hpp>
#include <iswapchain.hpp>
#include <iresource_manager.hpp>
#include <types.hpp>

class IGraphicsContext
{
public:
    virtual std::shared_ptr<IPipeline> createPipeline(IPipeline::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) const = 0;
    virtual std::shared_ptr<ISwapchain> createSwapchain(
        ISwapchain::CreateInfo createInfo) const = 0;

    virtual IResourceManager& resources() const = 0;

    virtual Multisampling maxSampleCount() const = 0;

    virtual void waitIdle() = 0;

    virtual ~IGraphicsContext() {}
};
