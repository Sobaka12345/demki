#include "renderer.hpp"

#include "graphics_context.hpp"
#include "swapchain.hpp"

#include "handles/render_pass.hpp"

#include <operation_context.hpp>

#include <optional>

namespace renderer::vk {

using namespace handles;

Renderer::Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo)
    : m_context(context)
    , m_multisampling(toVkSampleFlagBits(createInfo.multisampling()))
{
    m_attachments.reserve(Attachment::COUNT);

    m_attachments.push_back(AttachmentDescription{}
            .samples(m_multisampling)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_STORE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(m_multisampling == VK_SAMPLE_COUNT_1_BIT ?
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));

    m_attachments.push_back(AttachmentDescription{}
            .samples(m_multisampling)
            .loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
            .storeOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
            .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .finalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

    if (m_multisampling > VK_SAMPLE_COUNT_1_BIT)
    {
        m_attachments.push_back(AttachmentDescription{}
                .samples(VK_SAMPLE_COUNT_1_BIT)
                .loadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
                .storeOp(VK_ATTACHMENT_STORE_OP_STORE)
                .stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
                .stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
                .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
                .finalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
    }
}

Renderer::~Renderer()
{
    m_renderPasses.destroyAll(m_context.device());
}

renderer::OperationContext Renderer::start(IRenderTarget& target)
{
    renderer::OperationContext result;
    result.emplace<vk::OperationContext>(this);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    auto& specificContext = get(result);

    const std::array<VkClearValue, 2> clearValues{
        VkClearValue{ { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a } },
        VkClearValue{ { 1.0f, 0 } }
    };

    const auto renderPassInfo =
        RenderPassBeginInfo{}
            .renderPass(specificContext.renderPass)
            .framebuffer(specificContext.framebuffer)
			.renderArea(
				VkRect2D{ VkOffset2D{ 0, 0 }, VkExtent2D{ target.width(), target.height() } })
            .clearValueCount(clearValues.size())
            .pClearValues(clearValues.data());

    vkCmdBeginRenderPass(
        specificContext.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return result;
}

void Renderer::finish(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    vkCmdEndRenderPass(specContext.commandBuffer);

    context.operationTarget().present(context);
}

VkDevice Renderer::device() const
{
    return m_context.device();
}

VkSampleCountFlagBits Renderer::sampleCount() const
{
    return m_multisampling;
}

std::span<const AttachmentDescription> renderer::vk::Renderer::attachments() const
{
    return m_attachments;
}

VkRenderPass Renderer::renderPass(OperationContext& context)
{
    const auto target = context.specificTarget->toBase();
    if (auto el = m_renderPasses.find(target); el != m_renderPasses.end())
    {
        return el->second;
    }

    std::optional<AttachmentReference> colorAttachmentRef;
    std::optional<AttachmentReference> depthAttachmentRef;
    std::optional<AttachmentReference> colorAttachmentResolveRef;

    if (m_attachments.size() > Attachment::COLOR)
    {
        m_attachments[Attachment::COLOR].format(context.imageFormat);
        colorAttachmentRef.emplace(AttachmentReference{}
                .attachment(Attachment::COLOR)
                .layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    }

    if (m_attachments.size() > Attachment::DEPTH)
    {
        m_attachments[Attachment::DEPTH].format(context.depthFormat);
        depthAttachmentRef.emplace(AttachmentReference{}
                .attachment(Attachment::DEPTH)
                .layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    }

    if (m_attachments.size() > Attachment::RESOLVE)
    {
        m_attachments[Attachment::RESOLVE].format(context.imageFormat);
        colorAttachmentResolveRef.emplace(AttachmentReference{}
                .attachment(Attachment::RESOLVE)
                .layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
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

    const auto renderPassCreateInfo =
        RenderPassCreateInfo()
            .attachmentCount(m_attachments.size())
            .pAttachments(m_attachments.data())
            .subpassCount(1)
            .pSubpasses(&subpass)
            .dependencyCount(1)
            .pDependencies(&dependency);

    auto [result, emplaced] = m_renderPasses.emplace(target,
        RenderPass::create(m_context.device(), &renderPassCreateInfo, nullptr));

    DASSERT(emplaced);

    return result->second;
}

}    //  namespace renderer::vk
