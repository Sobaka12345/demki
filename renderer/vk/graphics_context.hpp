#pragma once

#include "handles/device.hpp"
#include "handles/debug_utils_messenger.hpp"
#include "handles/instance.hpp"

#include "buffer_shader_resource.hpp"

#include <igraphics_context.hpp>

#include <memory>

namespace shell {
class IVulkanWindow;
class IResources;
}

namespace renderer { namespace vk {

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
public:
    const static bool s_enableValidationLayers;
    const static std::vector<const char*> s_validationLayers;

public:
    GraphicsContext(shell::IVulkanWindow& window, shell::IResources& resources);
    GraphicsContext(GraphicsContext&& other) = delete;
    GraphicsContext(const GraphicsContext& other) = delete;
    virtual ~GraphicsContext();

public:
	std::weak_ptr<handles::Memory> fetchMemory(size_t size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    std::shared_ptr<ShaderInterfaceHandle> fetchHandleSpecific(ShaderBlockType sbt,
        uint32_t layoutSize);
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

    virtual std::shared_ptr<IModel> createModel(std::filesystem::path path) override;
    virtual std::shared_ptr<IModel> createModel(IModel::CreateInfo createInfo) override;
    virtual std::shared_ptr<ITexture> createTexture(std::filesystem::path path) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;

    virtual void waitIdle() override;

    virtual Multisampling maxSampleCount() const override;

    VkSurfaceKHR surface() const;
    const handles::Device& device() const;
    const shell::IWindow& window() const;

    VkFormat findDepthFormat() const;

private:
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) const;
    bool hasStencilComponent(VkFormat format) const;
    uint32_t dynamicAlignment(uint32_t layoutSize) const;

private:
    shell::IVulkanWindow& m_window;
    shell::IResources& m_resources;

    handles::HandleVector<handles::Buffer> m_buffers;

    std::unordered_map<uint32_t, StaticUniformBufferShaderResource> m_staticUniformShaderResources;
    std::unordered_map<uint32_t, DynamicUniformBufferShaderResource>
        m_dynamicUniformShaderResources;
    std::unordered_map<uint32_t, StorageBufferShaderResource> m_storageShaderResources;

    std::unique_ptr<handles::Surface> m_surface;
    std::unique_ptr<handles::Device> m_device;
    std::unique_ptr<handles::DebugUtilsMessenger> m_debugMessenger;
};

}}    //  namespace renderer::vk
