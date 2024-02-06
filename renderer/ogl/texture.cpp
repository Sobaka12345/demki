#include "texture.hpp"

#include "shader_interface_handle.hpp"

namespace ogl {

Texture::Texture(GraphicsContext& context, CreateInfo createInfo) noexcept
    : m_context(context)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    //  set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, createInfo.width, createInfo.height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, createInfo.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    m_handle = std::make_shared<TextureInterfaceHandle>(m_texture);
}

Texture::~Texture() {}

std::shared_ptr<IShaderInterfaceHandle> Texture::uniformHandle()
{
    return m_handle;
}

}    //  namespace ogl
