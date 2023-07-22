#include "renderer.hpp"

#include "graphics_context.hpp"
#include "swapchain.hpp"

#include "handles/creators.hpp"
#include "handles/render_pass.hpp"

#include <render_context.hpp>

namespace vk {

struct RenderInfoVisitor : public ::RenderInfoVisitor
{
    virtual void populateRenderInfo(const vk::Swapchain& swapchain)
    {
        depthFormat = swapchain.depthFormat();
        imageFormat = swapchain.imageFormat();
    };

    VkFormat imageFormat;
    VkFormat depthFormat;
};

Renderer::Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo)
    : m_context(context)
{
    //  TO DO CREATE INFO
    (void)createInfo;
}

::RenderContext Renderer::start(IRenderTarget& target)
{
    ::RenderContext result = vk::RenderContext{ .renderPass = &renderPass(target) };

    target.populateRenderContext(result);
    const auto& kek = get(result);

    const std::array<VkClearValue, 2> clearValues{ VkClearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } },
        VkClearValue{ { 1.0f, 0 } } };

    const auto renderPassInfo =
        handles::RenderPassBeginInfo{}
            .renderPass(*kek.renderPass)
            .framebuffer(*kek.framebuffer)
			.renderArea(
				VkRect2D{ VkOffset2D{ 0, 0 }, VkExtent2D{ target.width(), target.height() } })
            .clearValueCount(clearValues.size())
            .pClearValues(clearValues.data());

    vkCmdBeginRenderPass(*kek.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return result;
}

IRenderer& Renderer::addRenderTarget(IRenderTarget& target)
{
    ASSERT(!m_renderPasses.contains(&target), "render target already exists");

    RenderInfoVisitor renderInfo;
    target.accept(renderInfo);

    const auto colorAttachment =
        handles::AttachmentDescription{}
            .format(renderInfo.imageFormat)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_STORE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    const auto depthAttachment =
        handles::AttachmentDescription{}
            .format(renderInfo.depthFormat)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    const auto colorAttachmentRef = handles::AttachmentReference{}.attachment(0).layout(
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    const auto depthAttachmentRef = handles::AttachmentReference{}.attachment(1).layout(
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    const auto subpass =
        handles::SubpassDescription{}
            .pipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
            .colorAttachmentCount(1)
            .pColorAttachments(&colorAttachmentRef)
            .pDepthStencilAttachment(&depthAttachmentRef);

    const auto dependency =
        handles::SubpassDependency{}
            .srcSubpass(VK_SUBPASS_EXTERNAL)
            .dstSubpass(0)
            .srcStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
            .srcAccessMask(0)
            .dstStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
            .dstAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    m_renderPasses.emplace(&target,
        handles::RenderPass{ device(),
            handles::RenderPassCreateInfo()
                .attachmentCount(attachments.size())
                .pAttachments(attachments.data())
                .subpassCount(1)
                .pSubpasses(&subpass)
                .dependencyCount(1)
                .pDependencies(&dependency) });

    return *this;
}

const handles::Device& Renderer::device() const
{
    return m_context.device();
}

handles::RenderPass& Renderer::renderPass(IRenderTarget& target)
{
    if (auto el = m_renderPasses.find(&target); el != m_renderPasses.end())
    {
        return el->second;
    }

    addRenderTarget(target);

    return m_renderPasses.at(&target);
}

}    //  namespace vk
