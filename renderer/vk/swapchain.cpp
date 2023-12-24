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

Swapchain::Swapchain(const GraphicsContext& context, ISwapchain::CreateInfo _createInfo)
    : m_context(context)
    , m_swapchainInfo(std::move(_createInfo))
    , m_depthFormat(context.findDepthFormat())
    , m_currentFrame(0)
    , m_currentImage(0)
    , m_needRecreate(false)
{
    m_maxFramesInFlight = m_swapchainInfo.framesInFlight;
    m_context.window().registerFramebufferResizeCallback([this](int, int) {
        m_needRecreate = true;
    });

    m_resourcesInUse.resize(m_maxFramesInFlight);

    constexpr handles::FenceCreateInfo fenceInfo =
        handles::FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT);

    for (size_t i = 0; i < m_maxFramesInFlight; ++i)
    {
        m_inFlightFences.emplaceBack(m_context.device(),
            handles::FenceCreateInfo{}.flags(VK_FENCE_CREATE_SIGNALED_BIT));
        m_imageAvailableSemaphores.emplaceBack(m_context.device(), handles::SemaphoreCreateInfo{});
        m_renderFinishedSemaphores.emplaceBack(m_context.device(), handles::SemaphoreCreateInfo{});
    }

    m_commandBuffers =
        m_context.device()
            .commandPool(handles::GRAPHICS_COMPUTE)
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

    static std::vector<uint32_t> queueFamilyIndices = {};
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_context.device().queueFamilies()[handles::GRAPHICS_COMPUTE] !=
        m_context.device().queueFamilies()[handles::PRESENT])
    {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = { m_context.device().queueFamilies()[handles::GRAPHICS_COMPUTE],
            m_context.device().queueFamilies()[handles::PRESENT] };
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

void Swapchain::populateOperationContext(OperationContext& context)
{
    context.renderTarget = this;
    context.commandBuffer = &currentCommandBuffer();

    if (!m_swapChainFramebuffers.size())
    {
        for (size_t i = 0; i < m_swapChainImageViews.size(); ++i)
        {
            //  TO DO: remove this cringe
            std::vector<VkImageView> attachments;
            attachments.reserve(context.renderPass->attachments().size());
            for (auto attachment : context.renderPass->attachments())
            {
                if (attachment.finalLayout() == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                {
                    attachments.push_back(m_swapChainImageViews[i]);
                }
                else if (attachment.finalLayout() ==
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                {
                    if (!m_depthImage)
                    {
                        m_depthImage = std::make_unique<handles::Image>(m_context.device(),
                            imageCreateInfo()
                                .format(m_depthFormat)
                                .usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                                .samples(attachment.samples()));
                        m_depthImage->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                        auto depthViewInfo =
                            imageViewCreateInfo().image(*m_depthImage).format(m_depthFormat);
                        depthViewInfo.subresourceRange().aspectMask(VK_IMAGE_ASPECT_DEPTH_BIT);
                        m_depthImageView = std::make_unique<handles::ImageView>(m_context.device(),
                            std::move(depthViewInfo));
                    }
                    attachments.push_back(*m_depthImageView);
                }
                else if (attachment.finalLayout() == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    if (!m_colorImage)
                    {
                        m_colorImage = std::make_unique<handles::Image>(m_context.device(),
                            imageCreateInfo()
                                .format(m_swapchain->imageFormat())
                                .usage(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                .samples(attachment.samples()));
                        m_colorImage->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                        auto colorViewInfo =
                            imageViewCreateInfo()
                                .image(*m_colorImage)
                                .format(m_swapchain->imageFormat());
                        colorViewInfo.subresourceRange().aspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
                        m_colorImageView = std::make_unique<handles::ImageView>(m_context.device(),
                            std::move(colorViewInfo));
                    }
                    attachments.push_back(*m_colorImageView);
                }
            }

            const auto framebufferInfo =
                handles::FramebufferCreateInfo{}
                    .renderPass(*context.renderPass)
                    .attachmentCount(attachments.size())
                    .pAttachments(attachments.data())
                    .width(m_swapchainCreateInfo.imageExtent().width)
                    .height(m_swapchainCreateInfo.imageExtent().height)
                    .layers(1);

            m_swapChainFramebuffers.emplaceBack(m_context.device(), framebufferInfo);
        }
    }

    context.framebuffer = &currentFramebuffer();
}

bool Swapchain::prepare(::OperationContext& context)
{
    auto& specContext = get(context);

    vkWaitForFences(m_context.device(), 1, m_inFlightFences[m_currentFrame].handlePtr(), VK_TRUE,
        UINT64_MAX);

    m_resourcesInUse[m_currentFrame].sets.clear();

    VkResult result = vkAcquireNextImageKHR(m_context.device(), *m_swapchain, UINT64_MAX,
        m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImage);

    ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swap chain image!");


    populateOperationContext(specContext);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        return false;
    }

    vkResetFences(m_context.device(), 1, m_inFlightFences[m_currentFrame].handlePtr());

    const auto& commandBuffer = *specContext.commandBuffer;

    commandBuffer.reset();
    ASSERT(commandBuffer.begin() == VK_SUCCESS, "failed to begin recording command buffer!");

    return true;
}

void Swapchain::present(::OperationContext& context)
{
    const auto& commandBuffer = *get(context).commandBuffer;
    ASSERT(commandBuffer.end() == VK_SUCCESS, "failed to record command buffer!");

    m_resourcesInUse[m_currentFrame] = std::move(commandBuffer.resourcesInUse());

    std::vector<VkPipelineStageFlags> waitStages = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    auto submitInfo =
        handles::SubmitInfo{}
            .commandBufferCount(1)
            .pCommandBuffers(commandBuffer.handlePtr())
            .signalSemaphoreCount(1)
            .pSignalSemaphores(m_renderFinishedSemaphores[m_currentFrame].handlePtr());

    std::vector<VkSemaphore> waitSemaphores{ m_imageAvailableSemaphores[m_currentFrame].handle() };

    if (!m_renderWaitSemaphores.empty())
    {
        std::copy(m_renderWaitSemaphores.begin(), m_renderWaitSemaphores.end(),
            std::back_inserter(waitSemaphores));
        waitStages.push_back(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
        m_renderWaitSemaphores.clear();
    }

    submitInfo.waitSemaphoreCount(waitSemaphores.size())
        .pWaitSemaphores(waitSemaphores.data())
        .pWaitDstStageMask(waitStages.data());

    ASSERT(m_context.device()
                .queue(handles::GRAPHICS_COMPUTE)
                .lock()
                ->submit(1, &submitInfo, m_inFlightFences[m_currentFrame]) == VK_SUCCESS,
        "failed to submit draw command buffer!");

    VkResult result =
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

uint32_t Swapchain::framesInFlight() const
{
    return m_maxFramesInFlight;
}

uint32_t Swapchain::width() const
{
    return m_swapchainCreateInfo.imageExtent().width;
}

uint32_t Swapchain::height() const
{
    return m_swapchainCreateInfo.imageExtent().height;
}

int Swapchain::maxFramesInFlight() const
{
    return m_maxFramesInFlight;
}

VkFormat Swapchain::imageFormat() const
{
    return m_swapchainCreateInfo.imageFormat();
}

VkFormat Swapchain::depthFormat() const
{
    return m_depthFormat;
}

void Swapchain::populateWaitInfo(OperationContext& context)
{
    context.waitSemaphores.push_back(m_renderFinishedSemaphores[m_currentFrame].handle());
}

void Swapchain::waitFor(OperationContext& context)
{
    std::copy(context.waitSemaphores.begin(), context.waitSemaphores.end(),
        std::back_inserter(m_renderWaitSemaphores));
}

uint32_t Swapchain::descriptorsRequired() const
{
    return m_maxFramesInFlight;
}

handles::Framebuffer& Swapchain::currentFramebuffer()
{
    return m_swapChainFramebuffers[m_currentImage];
}

handles::CommandBuffer& Swapchain::currentCommandBuffer()
{
    return m_commandBuffers[m_currentFrame];
}

handles::ImageCreateInfo Swapchain::imageCreateInfo() const
{
    return handles::ImageCreateInfo{}
        .imageType(VK_IMAGE_TYPE_2D)
        .extent(VkExtent3D{
            m_swapchainCreateInfo.imageExtent().width,
            m_swapchainCreateInfo.imageExtent().height,
            1,
        })
        .mipLevels(1)
        .arrayLayers(1)
        .tiling(VK_IMAGE_TILING_OPTIMAL)
        .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
        .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
}

handles::ImageViewCreateInfo Swapchain::imageViewCreateInfo() const
{
    return handles::ImageViewCreateInfo()
        .viewType(VK_IMAGE_VIEW_TYPE_2D)
        .subresourceRange(
            ImageSubresourceRange{}.baseArrayLayer(0).layerCount(1).baseMipLevel(0).levelCount(1));
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
    m_colorImageView.reset();
    m_colorImage.reset();

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
        auto createInfo =
            imageViewCreateInfo().image(m_swapChainImages[i]).format(m_swapchain->imageFormat());
        createInfo.subresourceRange().aspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
        m_swapChainImageViews.emplaceBack(m_swapchain->device(), std::move(createInfo));
    }
}

}    //  namespace vk
