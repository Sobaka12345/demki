#include "renderer.hpp"

#include "graphics_context.hpp"
#include "swapchain.hpp"

#include "handles/render_pass.hpp"

#include <operation_context.hpp>

namespace vk {

struct RenderInfoVisitor : public ::RenderInfoVisitor
{
    virtual void populateRenderInfo(const vk::Swapchain& swapchain) override
    {
        depthFormat = swapchain.depthFormat();
        imageFormat = swapchain.imageFormat();
    };

    VkFormat imageFormat;
    VkFormat depthFormat;
};

Renderer::Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo)
    : m_context(context)
    , m_multisampling(toVkSampleFlagBits(createInfo.multisampling))
{}

::OperationContext Renderer::start(IRenderTarget& target)
{
    ::OperationContext result;
    result.emplace<vk::OperationContext>(this);

    auto& kek = get(result);
    kek.renderPass = &renderPass(target);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }


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

void Renderer::finish(::OperationContext& context)
{
    auto& specContext = get(context);
    vkCmdEndRenderPass(*specContext.commandBuffer);
    specContext.renderTarget->present(context);
}

IRenderer& Renderer::addRenderTarget(IRenderTarget& target)
{
    ASSERT(!m_renderPasses.contains(&target), "render target already exists");

    RenderInfoVisitor renderInfo;
    target.accept(renderInfo);

    std::vector<VkAttachmentDescription> attachments;
    std::optional<vk::AttachmentReference> colorAttachmentRef;
    std::optional<vk::AttachmentReference> depthAttachmentRef;
    std::optional<vk::AttachmentReference> colorAttachmentResolveRef;

    attachments.emplace_back(
        //  color attachment
        AttachmentDescription{}
            .format(renderInfo.imageFormat)
            .samples(m_multisampling)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_STORE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(m_multisampling == VK_SAMPLE_COUNT_1_BIT ?
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    colorAttachmentRef.emplace(
        AttachmentReference{}.attachment(0).layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));

    attachments.emplace_back(
        //  depth attachment =
        AttachmentDescription{}
            .format(renderInfo.depthFormat)
            .samples(m_multisampling)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    depthAttachmentRef.emplace(AttachmentReference{}.attachment(1).layout(
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

    if (m_multisampling > VK_SAMPLE_COUNT_1_BIT)
    {
        attachments.emplace_back(
            //  color attachment resolve
            AttachmentDescription{}
                .format(renderInfo.imageFormat)
                .samples(VK_SAMPLE_COUNT_1_BIT)
                .loadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
                .storeOp(VK_ATTACHMENT_STORE_OP_STORE)
                .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
                .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
                .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
                .finalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
        colorAttachmentResolveRef.emplace(
            AttachmentReference{}.attachment(2).layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    }

    const auto subpass =
        SubpassDescription{}
            .pipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
            .colorAttachmentCount(1)
            .pColorAttachments(
                colorAttachmentRef.has_value() ? &colorAttachmentRef.value() : nullptr)
            .pDepthStencilAttachment(
                depthAttachmentRef.has_value() ? &depthAttachmentRef.value() : nullptr)
            .pResolveAttachments(colorAttachmentResolveRef.has_value() ?
                    &colorAttachmentResolveRef.value() :
                    nullptr);

    const auto dependency =
        SubpassDependency{}
            .srcSubpass(VK_SUBPASS_EXTERNAL)
            .dstSubpass(0)
            .srcStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
            .srcAccessMask(0)
            .dstStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
            .dstAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

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

VkSampleCountFlagBits Renderer::sampleCount() const
{
    return m_multisampling;
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
