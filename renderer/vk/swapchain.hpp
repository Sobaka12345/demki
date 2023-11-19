#pragma once

#include "render_target.hpp"

#include "handles/fence.hpp"
#include "handles/semaphore.hpp"
#include "handles/swapchain.hpp"
#include "handles/image_view.hpp"
#include "handles/framebuffer.hpp"

#include <iswapchain.hpp>

#include <memory>

class Window;

namespace vk {

class GraphicsContext;

namespace handles {
class Surface;
}

class Swapchain
    : public ISwapchain
    , public RenderTarget
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities,
        const Window& window);
    static VkSurfaceFormatKHR chooseSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR choosePresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    static SwapChainSupportDetails supportDetails(VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);

public:
    Swapchain(const GraphicsContext& context, ISwapchain::CreateInfo createInfo);
    ~Swapchain();

    virtual bool prepare(::OperationContext& context) override;
    virtual void present(::OperationContext& context) override;

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

private:
    void recreate();
    void destroy();
    void create();
    void populateOperationContext(OperationContext& context);

    handles::Framebuffer& currentFramebuffer();
    handles::CommandBuffer& currentCommandBuffer();

    handles::ImageCreateInfo imageCreateInfo() const;
    handles::ImageViewCreateInfo imageViewCreateInfo() const;

private:
    const GraphicsContext& m_context;
    ISwapchain::CreateInfo m_swapchainInfo;
    VkFormat m_depthFormat;
    handles::SwapchainCreateInfoKHR m_swapchainCreateInfo;


    std::unique_ptr<handles::Image> m_colorImage;
    std::unique_ptr<handles::ImageView> m_colorImageView;
    std::unique_ptr<handles::Image> m_depthImage;
    std::unique_ptr<handles::ImageView> m_depthImageView;
    std::unique_ptr<handles::Swapchain> m_swapchain;

    std::function<void(Swapchain&)> m_drawCallback;

    bool m_needRecreate;

    uint32_t m_currentImage;
    int m_currentFrame;
    int m_maxFramesInFlight;

    handles::HandleVector<handles::CommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_renderWaitSemaphores;
    handles::HandleVector<handles::Semaphore> m_imageAvailableSemaphores;
    handles::HandleVector<handles::Semaphore> m_renderFinishedSemaphores;
    handles::HandleVector<handles::Fence> m_inFlightFences;

    handles::HandleVector<handles::Image> m_swapChainImages;
    handles::HandleVector<handles::ImageView> m_swapChainImageViews;
    mutable handles::HandleVector<handles::Framebuffer> m_swapChainFramebuffers;
};

}    //  namespace vk
