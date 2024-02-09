#pragma once

#include <igraphics_context.hpp>
#include <iresources.hpp>
#include <iopengl_window.hpp>

namespace renderer::ogl {

class ResourceManager;

class GraphicsContext : public IGraphicsContext
{
public:
    GraphicsContext(shell::IOpenGLWindow& window, shell::IResources& resources);
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

    const shell::IWindow& window() const;
    shell::IWindow& window();

    virtual std::shared_ptr<IModel> createModel(std::filesystem::path path) override;
    virtual std::shared_ptr<IModel> createModel(IModel::CreateInfo createInfo) override;
    virtual std::shared_ptr<ITexture> createTexture(std::filesystem::path path) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;


private:
    shell::IOpenGLWindow& m_window;
};

}    //  namespace renderer::ogl
