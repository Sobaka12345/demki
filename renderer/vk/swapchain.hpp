#pragma once

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

class Swapchain : public ISwapchain
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
    Swapchain(const GraphicsContext& context, VkFormat depthFormat);
    ~Swapchain();

    virtual void setDrawCallback(std::function<void(IRenderTarget&)> callback) override;
    virtual void present() override;

    virtual void accept(RenderInfoVisitor& visitor) const override;
    virtual void populateRenderContext(::RenderContext& context) override;

    virtual uint32_t width() const override;
    virtual uint32_t height() const override;

    VkFormat imageFormat() const;
    VkFormat depthFormat() const;

private:
    void recreate();
    void destroy();
    void create();

    handles::Framebuffer& currentFramebuffer();
    handles::CommandBuffer& currentCommandBuffer();

private:
    const GraphicsContext& m_context;
    VkFormat m_depthFormat;
    handles::SwapchainCreateInfoKHR m_swapchainCreateInfo;

    std::unique_ptr<handles::Image> m_depthImage;
    std::unique_ptr<handles::ImageView> m_depthImageView;
    std::unique_ptr<handles::Swapchain> m_swapchain;

    std::function<void(Swapchain&)> m_drawCallback;

    bool m_needRecreate;

    uint32_t m_currentImage;
    int m_currentFrame;
    int m_maxFramesInFlight;
    handles::HandleVector<handles::CommandBuffer> m_commandBuffers;
    handles::HandleVector<handles::Semaphore> m_imageAvailableSemaphores;
    handles::HandleVector<handles::Semaphore> m_renderFinishedSemaphores;
    handles::HandleVector<handles::Fence> m_inFlightFences;

    handles::HandleVector<handles::Image> m_swapChainImages;
    handles::HandleVector<handles::ImageView> m_swapChainImageViews;
    mutable handles::HandleVector<handles::Framebuffer> m_swapChainFramebuffers;
};

}    //  namespace vk
