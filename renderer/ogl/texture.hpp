#pragma once

#include <itexture.hpp>

#include <glad/glad.h>

namespace ogl {

class GraphicsContext;
class ShaderInterfaceHandle;

class Texture : public ITexture
{
public:
    explicit Texture(GraphicsContext& context, ITexture::CreateInfo createInfo) noexcept;
    virtual ~Texture();

    virtual std::shared_ptr<IShaderInterfaceHandle> uniformHandle() override;

private:
    GraphicsContext& m_context;
    GLuint m_texture;

    std::shared_ptr<ShaderInterfaceHandle> m_handle;
};

}    //  namespace ogl
