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
    virtual ::RenderContext start(IRenderTarget& target) override;
    virtual IRenderer& addRenderTarget(IRenderTarget& target) override;

    const handles::Device& device() const;

private:
    handles::RenderPass& renderPass(IRenderTarget& target);

private:
    const GraphicsContext& m_context;
    std::map<const IRenderTarget*, handles::RenderPass> m_renderPasses;
};

}    //  namespace vk
