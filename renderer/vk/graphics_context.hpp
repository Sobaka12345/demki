#pragma once

#include "handles/device.hpp"
#include "handles/debug_utils_messenger.hpp"
#include "handles/instance.hpp"

#include <igraphics_context.hpp>

#include <memory>

class Window;

namespace vk {

namespace handles {
class RenderPass;
class Surface;
class Swapchain;
}

class ResourceManager;

class GraphicsContext
    : public handles::Instance
    , public IGraphicsContext
{
protected:
    using TimeResolution = std::nano;

public:
    const static bool s_enableValidationLayers;
    const static std::vector<const char*> s_validationLayers;

public:
    GraphicsContext(const Window& window);
    GraphicsContext(GraphicsContext&& other) = delete;
    GraphicsContext(const GraphicsContext& other) = delete;
    virtual ~GraphicsContext();

public:
    virtual std::shared_ptr<IComputer> createComputer(
        IComputer::CreateInfo createInfo) const override;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) const override;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) const override;
    virtual std::shared_ptr<IRenderer> createRenderer(
        IRenderer::CreateInfo createInfo) const override;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) const override;
    virtual std::shared_ptr<ISwapchain> createSwapchain(
        ISwapchain::CreateInfo createInfo) const override;

    ResourceManager& resourcesSpecific() const;
    virtual IResourceManager& resources() const override;

    virtual void waitIdle() override;

    virtual Multisampling maxSampleCount() const override;

    ResourceManager& resources();

    const handles::Surface& surface() const;
    const handles::Device& device() const;
    const Window& window() const;

    VkFormat findDepthFormat() const;

private:
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) const;
    bool hasStencilComponent(VkFormat format) const;

private:
    const Window& m_window;

    std::unique_ptr<handles::Surface> m_surface;
    std::unique_ptr<handles::Device> m_device;
    std::unique_ptr<handles::DebugUtilsMessenger> m_debugMessenger;

    std::unique_ptr<ResourceManager> m_resourceManager;
};

}    //  namespace vk
