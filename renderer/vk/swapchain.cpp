#include "swapchain.hpp"

#include "graphics_context.hpp"

#include "handles/command_pool.hpp"
#include "handles/queue.hpp"
#include "handles/render_pass.hpp"
#include "handles/surface.hpp"

#include <irenderer.hpp>
#include <window.hpp>

#include <GLFW/glfw3.h>

namespace vk {

VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities,
    const Window& window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        const auto [width, height] = window.framebufferSize();
        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::choosePresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

Swapchain::SwapChainSupportDetails Swapchain::supportDetails(VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
            details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount,
            details.presentModes.data());
    }

    return details;
}

Swapchain::Swapchain(const GraphicsContext& context, VkFormat depthFormat)
    : m_context(context)
    , m_depthFormat(depthFormat)
    , m_currentFrame(0)
    , m_currentImage(0)
    , m_maxFramesInFlight(2)
    , m_needRecreate(false)
{
    m_context.window().registerFramebufferResizeCallback([this](int, int) {
        m_needRecreate = true;
    });

    constexpr handles::FenceCreateInfo fenceInfo =
        handles::FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT);

    for (size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_inFlightFences.emplace_back(m_context.device(),
            handles::FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT));
        m_imageAvailableSemaphores.emplace_back(m_context.device(), handles::SemaphoreCreateInfo{});
        m_renderFinishedSemaphores.emplace_back(m_context.device(), handles::SemaphoreCreateInfo{});
    }

    m_commandBuffers =
        m_context.device()
            .commandPool(handles::GRAPHICS)
            .lock()
            ->allocateBuffers(m_maxFramesInFlight);

    const auto supportDetails =
        Swapchain::supportDetails(m_context.device().physicalDevice(), m_context.surface());

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode = choosePresentMode(supportDetails.presentModes);
    VkExtent2D extent = chooseExtent(supportDetails.capabilities, m_context.window());
    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
    if (supportDetails.capabilities.maxImageCount > 0 &&
        imageCount > supportDetails.capabilities.maxImageCount)
    {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    static std::span<const uint32_t> queueFamilyIndices = {};
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_context.device().queueFamilies()[handles::GRAPHICS] !=
        m_context.device().queueFamilies()[handles::PRESENT])
    {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = { m_context.device().queueFamilies().begin(),
            m_context.device().queueFamilies().end() };
    }

    m_swapchainCreateInfo =
        handles::SwapchainCreateInfoKHR{}
            .surface(m_context.surface())
            .minImageCount(imageCount)
            .imageFormat(surfaceFormat.format)
            .imageColorSpace(surfaceFormat.colorSpace)
            .imageExtent(extent)
            .imageArrayLayers(1)
            .imageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .imageSharingMode(sharingMode)
            .queueFamilyIndexCount(queueFamilyIndices.size())
            .pQueueFamilyIndices(queueFamilyIndices.data())
            .preTransform(supportDetails.capabilities.currentTransform)
            .compositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
            .presentMode(presentMode)
            .clipped(VK_TRUE)
            .oldSwapchain(VK_NULL_HANDLE);

    create();
}

Swapchain::~Swapchain()
{
    destroy();

    m_inFlightFences.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
}

void Swapchain::setDrawCallback(std::function<void(IRenderTarget&)> callback)
{
    m_drawCallback = callback;
}

void Swapchain::present()
{
    vkWaitForFences(m_context.device(), 1, m_inFlightFences[m_currentFrame].handlePtr(), VK_TRUE,
        UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(m_context.device(), *m_swapchain, UINT64_MAX,
        m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImage);

    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swap chain image!");

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        return;
    }

    vkResetFences(m_context.device(), 1, m_inFlightFences[m_currentFrame].handlePtr());

    const auto& commandBuffer = m_commandBuffers[m_currentFrame];

    commandBuffer.reset();
    ASSERT(commandBuffer.begin() == VK_SUCCESS, "failed to begin recording command buffer!");
    m_drawCallback(*this);

    ASSERT(commandBuffer.end() == VK_SUCCESS, "failed to record command buffer!");

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    const auto submitInfo =
        handles::SubmitInfo{}
            .waitSemaphoreCount(1)
            .pWaitSemaphores(m_imageAvailableSemaphores[m_currentFrame].handlePtr())
            .pWaitDstStageMask(waitStages)
            .commandBufferCount(1)
            .pCommandBuffers(commandBuffer.handlePtr())
            .signalSemaphoreCount(1)
            .pSignalSemaphores(m_renderFinishedSemaphores[m_currentFrame].handlePtr());

    ASSERT(m_context.device()
                .queue(handles::GRAPHICS)
                .lock()
                ->submit(1, &submitInfo, m_inFlightFences[m_currentFrame]) == VK_SUCCESS,
        "failed to submit draw command buffer!");

    result =
        m_context.device()
            .queue(handles::PRESENT)
            .lock()
            ->presentKHR(
                handles::PresentInfoKHR{}
                    .waitSemaphoreCount(1)
                    .pWaitSemaphores(m_renderFinishedSemaphores[m_currentFrame].handlePtr())
                    .swapchainCount(1)
                    .pSwapchains(m_swapchain->handlePtr())
                    .pImageIndices(&m_currentImage));

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_needRecreate ||
        m_context.window().iconified())
    {
        recreate();
        m_needRecreate = false;
    }
    else if (result != VK_SUCCESS)
    {
        ASSERT(false, "failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

void Swapchain::accept(RenderInfoVisitor& visitor) const
{
    visitor.populateRenderInfo(*this);
}

void Swapchain::populateRenderContext(::RenderContext& context)
{
    auto& specContext = get(context);

    if (!m_swapChainFramebuffers.size())
    {
        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            const std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i],
                *m_depthImageView };

            const auto framebufferInfo =
                handles::FramebufferCreateInfo{}
                    .renderPass(*specContext.renderPass)
                    .attachmentCount(attachments.size())
                    .pAttachments(attachments.data())
                    .width(m_swapchainCreateInfo.imageExtent().width)
                    .height(m_swapchainCreateInfo.imageExtent().height)
                    .layers(1);

            m_swapChainFramebuffers.emplace_back(m_context.device(), framebufferInfo);
        }
    }

    specContext.framebuffer = &currentFramebuffer();
    specContext.commandBuffer = &currentCommandBuffer();
}

uint32_t Swapchain::width() const
{
    return m_swapchainCreateInfo.imageExtent().width;
}

uint32_t Swapchain::height() const
{
    return m_swapchainCreateInfo.imageExtent().height;
}

VkFormat Swapchain::imageFormat() const
{
    return m_swapchainCreateInfo.imageFormat();
}

VkFormat Swapchain::depthFormat() const
{
    return m_depthFormat;
}

handles::Framebuffer& Swapchain::currentFramebuffer()
{
    return m_swapChainFramebuffers[m_currentImage];
}

handles::CommandBuffer& Swapchain::currentCommandBuffer()
{
    return m_commandBuffers[m_currentFrame];
}

void Swapchain::recreate()
{
    while (m_context.window().iconified())
    {
        glfwWaitEvents();
    }
    m_context.device().waitIdle();

    destroy();
    const auto supportDetails =
        Swapchain::supportDetails(m_context.device().physicalDevice(), m_context.surface());
    m_swapchainCreateInfo.imageExtent(
        chooseExtent(supportDetails.capabilities, m_context.window()));
    create();
}

void Swapchain::destroy()
{
    m_depthImageView.reset();
    m_depthImage.reset();

    m_swapChainFramebuffers.clear();
    m_swapChainImageViews.clear();
    m_swapChainImages.clear();
    m_swapchain.reset();
}

void Swapchain::create()
{
    m_swapchain = std::make_unique<handles::Swapchain>(m_context.device(), m_swapchainCreateInfo);
    m_swapChainImages = m_swapchain->images();

    for (size_t i = 0; i < m_swapChainImages.size(); ++i)
    {
        m_swapChainImageViews.emplace_back(m_swapchain->device(),
            handles::ImageViewCreateInfo()
                .image(m_swapChainImages[i])
                .viewType(VK_IMAGE_VIEW_TYPE_2D)
                .format(m_swapchain->imageFormat())
                .subresourceRange(
                    ImageSubresourceRange{}
                        .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
                        .baseArrayLayer(0)
                        .layerCount(1)
                        .baseMipLevel(0)
                        .levelCount(1)));
    }

    m_depthImage = std::make_unique<handles::Image>(m_context.device(),
        handles::ImageCreateInfo{}
            .imageType(VK_IMAGE_TYPE_2D)
            .extent(VkExtent3D{
                m_swapchainCreateInfo.imageExtent().width,
                m_swapchainCreateInfo.imageExtent().height,
                1,
            })
            .mipLevels(1)
            .arrayLayers(1)
            .format(m_depthFormat)
            .tiling(VK_IMAGE_TILING_OPTIMAL)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_depthImage->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_depthImageView = std::make_unique<handles::ImageView>(m_context.device(),
        handles::ImageViewCreateInfo()
            .image(*m_depthImage)
            .viewType(VK_IMAGE_VIEW_TYPE_2D)
            .format(m_depthFormat)
            .subresourceRange(
                ImageSubresourceRange{}
                    .aspectMask(VK_IMAGE_ASPECT_DEPTH_BIT)
                    .baseArrayLayer(0)
                    .layerCount(1)
                    .baseMipLevel(0)
                    .levelCount(1)));
}

}    //  namespace vk
