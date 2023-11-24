#pragma once

#include "handles/render_pass.hpp"

#include <irenderer.hpp>

#include <map>

namespace vk {

class GraphicsContext;

namespace handles {
class Device;
}

class Renderer : public IRenderer
{
public:
    Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo);
    virtual ::OperationContext start(IRenderTarget& target) override;
    virtual void finish(::OperationContext& context) override;

    const handles::Device& device() const;
    VkSampleCountFlagBits sampleCount() const;

private:
    IRenderer& addRenderTarget(IRenderTarget& target);
    handles::RenderPass& renderPass(IRenderTarget& target);

private:
    const GraphicsContext& m_context;

    VkSampleCountFlagBits m_multisampling;
    std::unordered_map<size_t, IRenderTarget&> m_targets;
    std::map<const IRenderTarget*, handles::RenderPass> m_renderPasses;
};

}    //  namespace vk
