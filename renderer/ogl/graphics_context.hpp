#pragma once

#include <igraphics_context.hpp>
#include <iresources.hpp>

namespace renderer {
class IOpenGLSurface;
}

namespace renderer::ogl {

class ResourceManager;

class GraphicsContext : public IGraphicsContext
{
public:
    GraphicsContext(IOpenGLSurface& defaultSurface);
    GraphicsContext(GraphicsContext&& other) = delete;
    GraphicsContext(const GraphicsContext& other) = delete;
    virtual ~GraphicsContext();

public:
    std::shared_ptr<ShaderInterfaceHandle> fetchHandle(ShaderBlockType sbt, uint32_t layoutSize);
    std::shared_ptr<ISwapchain> createSwapchain(IOpenGLSurface& surface,
        ISwapchain::CreateInfo createInfo);

    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) override;

    virtual Multisampling maxSampleCount() const override;

    virtual void waitIdle() override;

    virtual std::shared_ptr<IMesh> createMesh(std::filesystem::path path) override;
    virtual std::shared_ptr<IMesh> createMesh(IMesh::CreateInfo createInfo) override;
    virtual std::shared_ptr<ITexture> createTexture(std::filesystem::path path) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;
    virtual std::shared_ptr<IUniformBuffer> createUniformBuffer(
        IUniformBuffer::CreateInfo createInfo) override;
};

}    //  namespace renderer::ogl
