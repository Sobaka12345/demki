#pragma once

#include "handles/command_buffer.hpp"
#include "handles/fence.hpp"
#include "handles/framebuffer.hpp"
#include "handles/image_view.hpp"
#include "handles/semaphore.hpp"
#include "handles/swapchain.hpp"

#include "specific_context_object.hpp"
#include "specific_operation_target.hpp"

#include <iswapchain.hpp>

namespace renderer {
class IVulkanSurface;

namespace vk {

class GraphicsContext;

namespace handles {
class Surface;
}

class Swapchain
    : public SpecificBase<ISwapchain, ISpecificContextObject, ISpecificOperationTarget>
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities,
        const IVulkanSurface& window);
    static VkSurfaceFormatKHR chooseSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR choosePresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    static SwapChainSupportDetails supportDetails(VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);

public:
    Swapchain(
        GraphicsContext& context, IVulkanSurface& surface, ISwapchain::CreateInfo createInfo);
    ~Swapchain();

    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual void accept(RenderInfoVisitor& visitor) const override;

    virtual uint32_t framesInFlight() const override;
    virtual uint32_t width() const override;
    virtual uint32_t height() const override;

    int maxFramesInFlight() const;
    VkFormat imageFormat() const;
    VkFormat depthFormat() const;
    VkSampleCountFlagBits sampleCount() const;

    virtual void populateWaitInfo(OperationContext& context) override;
    virtual void waitFor(OperationContext& context) override;

    virtual uint32_t currentFrameIndex() const override;

private:
    void recreate();
    void destroy();
    void create();
    void populateOperationContext(OperationContext& context);

    handles::Framebuffer currentFramebuffer();
    handles::CommandBuffer currentCommandBuffer();

    ImageCreateInfo imageCreateInfo() const;
    ImageViewCreateInfo imageViewCreateInfo() const;

private:
    IVulkanSurface& m_surface;

    ISwapchain::CreateInfo m_swapchainInfo;
    VkFormat m_depthFormat;
    SwapchainCreateInfoKHR m_swapchainCreateInfo;


    handles::Image m_colorImage;
    handles::ImageView m_colorImageView;
    handles::Image m_depthImage;
    handles::ImageView m_depthImageView;
    handles::SwapchainKHR m_swapchain;

    std::function<void(Swapchain&)> m_drawCallback;

    bool m_needRecreate;

    uint32_t m_currentImage;
    int m_currentFrame;
    int m_maxFramesInFlight;

    handles::CommandBufferContainer::Vector<> m_commandBuffers;

    handles::SemaphoreContainer::Vector<> m_renderWaitSemaphores;
    handles::SemaphoreContainer::Vector<> m_imageAvailableSemaphores;
    handles::SemaphoreContainer::Vector<> m_renderFinishedSemaphores;
    handles::FenceContainer::Vector<> m_inFlightFences;

    handles::ImageContainer::Vector<> m_swapChainImages;
    handles::ImageViewContainer::Vector<> m_swapChainImageViews;
    mutable handles::FramebufferContainer::Vector<> m_swapChainFramebuffers;
};

}    //  namespace vk
}    //  namespace renderer
