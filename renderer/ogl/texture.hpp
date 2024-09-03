#pragma once

#include <itexture.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

class GraphicsContext;

class Texture : public ITexture
{
public:
    explicit Texture(GraphicsContext& context, ITexture::CreateInfo createInfo) noexcept;
    virtual ~Texture();

    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) override;
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;

private:
    std::shared_ptr<ShaderInterfaceHandle> m_handle;

    GraphicsContext& m_context;
    GLuint m_texture;
};

}    //  namespace renderer::ogl
