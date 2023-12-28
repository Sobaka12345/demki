#pragma once

#include <igraphics_context.hpp>

class Window;

namespace ogl {

class ResourceManager;

class GraphicsContext : public IGraphicsContext
{
public:
    GraphicsContext(const Window& window);
    GraphicsContext(GraphicsContext&& other) = delete;
    GraphicsContext(const GraphicsContext& other) = delete;
    virtual ~GraphicsContext();

public:
    virtual std::shared_ptr<IShaderInterfaceHandle> fetchHandle(ShaderBlockType sbt,
        uint32_t layoutSize) override;

    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) override;
    virtual std::shared_ptr<ISwapchain> createSwapchain(ISwapchain::CreateInfo createInfo) override;

    virtual Multisampling maxSampleCount() const override;

    virtual void waitIdle() override;

    const Window& window() const;

    virtual std::shared_ptr<IModel> createModel(IModel::CreateInfo createInfo) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;

private:
    const Window& m_window;
};

}    //  namespace ogl
