#pragma once

#include "handles/device.hpp"
#include "handles/render_pass.hpp"

#include "types.hpp"

#include <irenderer.hpp>

namespace renderer::vk {

class GraphicsContext;

class Renderer : public IRenderer
{
    enum Attachment {
        BEGIN = 0,
        COLOR = BEGIN,
        DEPTH,
        RESOLVE,
        COUNT
    };

public:
    Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo);
    virtual ~Renderer() override;
    virtual renderer::OperationContext start(IRenderTarget& target) override;
    virtual void finish(renderer::OperationContext& context) override;

    VkDevice device() const;
    VkSampleCountFlagBits sampleCount() const;
    std::span<const AttachmentDescription> attachments() const;

    VkRenderPass renderPass(vk::OperationContext& context);

private:
    const GraphicsContext& m_context;

    std::vector<AttachmentDescription> m_attachments;
    VkSampleCountFlagBits m_multisampling;
    glm::vec4 m_clearColor;
    handles::RenderPass::Map<const IOperationTarget*> m_renderPasses;
};

}    //  namespace renderer::vk
