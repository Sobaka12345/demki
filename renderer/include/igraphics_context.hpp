#pragma once

#include <icomputer.hpp>
#include <icompute_pipeline.hpp>
#include <iuniform_set_pool.hpp>
#include <igraphics_pipeline.hpp>
#include <irenderer.hpp>
#include <iswapchain.hpp>
#include <istorage_buffer.hpp>
#include <iuniform_buffer.hpp>

#include <iresources.hpp>

namespace renderer {

class IGraphicsContext
{
public:
    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IUniformBuffer> createUniformBuffer(
        IUniformBuffer::CreateInfo createInfo) = 0;

    virtual Multisampling maxSampleCount() const = 0;

    virtual void waitIdle() = 0;

    virtual ~IGraphicsContext() {}

    template <typename T>
    auto create()
    {}

protected:
    using TimeResolution = std::nano;
};

}    //  namespace renderer
