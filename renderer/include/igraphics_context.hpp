#pragma once

#include <icomputer.hpp>
#include <icompute_pipeline.hpp>
#include <igraphics_pipeline.hpp>
#include <imodel.hpp>
#include <itexture.hpp>
#include <irenderer.hpp>
#include <iswapchain.hpp>
#include <istorage_buffer.hpp>
#include <ishader_interface_handle.hpp>

#include <iopengl_window.hpp>
#include <ivulkan_window.hpp>
#include <iresources.hpp>

namespace renderer {

class IGraphicsContext : public IShaderResourceProvider
{
public:
    template <typename WindowT>
    static std::unique_ptr<IGraphicsContext> create(WindowT& window, shell::IResources& resources)
    {
        if constexpr (std::derived_from<std::remove_reference_t<WindowT>, shell::IOpenGLWindow>)
        {
            return std::make_unique<ogl::GraphicsContext>(window, resources);
        }
        else if constexpr (
            std::derived_from<std::remove_reference_t<WindowT>, shell::IVulkanWindow>)
        {
            return std::make_unique<vk::GraphicsContext>(window, resources);
        }

        return nullptr;
    }

    static std::unique_ptr<IGraphicsContext> create(shell::IWindow& window,
        shell::IResources& resources);

public:
    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<ISwapchain> createSwapchain(ISwapchain::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IModel> createModel(IModel::CreateInfo createInfo) = 0;
    virtual std::shared_ptr<IModel> createModel(std::filesystem::path path) = 0;
    virtual std::shared_ptr<ITexture> createTexture(std::filesystem::path path) = 0;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) = 0;

    virtual Multisampling maxSampleCount() const = 0;

    virtual void waitIdle() = 0;

    virtual ~IGraphicsContext() {}

protected:
    using TimeResolution = std::nano;
};

}    //  namespace renderer
