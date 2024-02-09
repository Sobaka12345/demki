#pragma once

#include "handles/render_pass.hpp"

#include <irenderer.hpp>

#include <map>

namespace renderer::vk {

class GraphicsContext;

namespace handles {
class Device;
}

class Renderer : public IRenderer
{
public:
    Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo);
    virtual renderer::OperationContext start(IRenderTarget& target) override;
    virtual void finish(renderer::OperationContext& context) override;

    const handles::Device& device() const;
    VkSampleCountFlagBits sampleCount() const;

private:
    IRenderer& addRenderTarget(IRenderTarget& target);
    handles::RenderPass& renderPass(IRenderTarget& target);

private:
    const GraphicsContext& m_context;

    VkSampleCountFlagBits m_multisampling;
    glm::vec4 m_clearColor;
    std::map<const IRenderTarget*, handles::RenderPass> m_renderPasses;
};

}    //  namespace renderer::vk
