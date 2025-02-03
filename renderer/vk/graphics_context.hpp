#pragma once

#include "handles/command_pool.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/physical_device.hpp"
#include "handles/device.hpp"
#include "handles/instance.hpp"
#include "handles/queue.hpp"

#include <igraphics_context.hpp>

#include <memory>

namespace shell {
class IResources;
}

namespace renderer {
class IVulkanSurface;
}

namespace renderer { namespace vk {

class ResourceManager;

class GraphicsContext: public IGraphicsContext
{
public:
    const static bool s_enableValidationLayers;
    const static std::vector<const char*> s_validationLayers;
    const static std::vector<const char*> s_deviceExtensions;

public:
    GraphicsContext(ApplicationInfo appInfo);
    GraphicsContext(GraphicsContext&& other) = delete;
    GraphicsContext(const GraphicsContext& other) = delete;
    virtual ~GraphicsContext();

    //  TO DO: rework to provide device picking logic for any surface
    void init(IVulkanSurface& surface);

public:
    std::shared_ptr<ISwapchain> createSwapchain(IVulkanSurface& surface,
        ISwapchain::CreateInfo createInfo);
    virtual std::shared_ptr<IComputer> createComputer(IComputer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IComputePipeline> createComputePipeline(
        IComputePipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IGraphicsPipeline> createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo createInfo) override;
    virtual std::shared_ptr<IRenderer> createRenderer(IRenderer::CreateInfo createInfo) override;
    virtual std::shared_ptr<IStorageBuffer> createStorageBuffer(
        IStorageBuffer::CreateInfo createInfo) override;

    virtual std::shared_ptr<IMesh> createMesh(std::filesystem::path path) override;
    virtual std::shared_ptr<IMesh> createMesh(IMesh::CreateInfo createInfo) override;
    virtual std::shared_ptr<ITexture> createTexture(std::filesystem::path path) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;
    virtual std::shared_ptr<IUniformBuffer> createUniformBuffer(
        IUniformBuffer::CreateInfo createInfo) override;

    virtual void waitIdle() override;

    handles::DescriptorSetLayout descriptorSetLayout(uint32_t id) const;

    virtual Multisampling maxSampleCount() const override;

    handles::Instance instance() const;
    handles::Device device() const;
    uint32_t queueIndex(QueueFamilyType familyType) const;
    handles::Queue queue(QueueFamilyType familyType) const;
    handles::PhysicalDevice physicalDevice() const;
    const PhysicalDeviceInfo& physicalDeviceInfo() const;
    handles::CommandPool commandPool(QueueFamilyType type) const;

    VkFormat findDepthFormat() const;
    uint32_t dynamicAlignment(uint32_t layoutSize) const;

private:
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    handles::Device m_device;
    handles::CommandPoolContainer::Array<enumT(QueueFamilyType::COUNT)> m_commandPools;
    handles::QueueContainer::Array<enumT(QueueFamilyType::COUNT)> m_queues;
    std::vector<std::pair<handles::PhysicalDevice, PhysicalDeviceInfo>> m_physicalDevices;
    handles::DescriptorSetLayoutContainer::HashMap<uint32_t> m_layouts;
};

}}    //  namespace renderer::vk
