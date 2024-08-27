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

    //  IShaderResource interface
    virtual std::shared_ptr<IShaderInterfaceHandle> handle() override;

private:
    std::shared_ptr<ShaderInterfaceHandle> m_handle;

    GraphicsContext& m_context;
    GLuint m_texture;
};

}    //  namespace renderer::ogl
