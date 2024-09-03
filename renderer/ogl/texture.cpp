#include "texture.hpp"

#include "graphics_context.hpp"
#include "pipeline.hpp"

namespace renderer::ogl {

Texture::Texture(GraphicsContext& context, CreateInfo createInfo) noexcept
    : m_context(context)
{
    //  m_handle = context.fetchHandle(ShaderBlockType::SAMPLER, createInfo.imageSize);

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
}

Texture::~Texture() {}

void Texture::adapt(renderer::OperationContext& context, uint32_t bindingId)
{
    //  NOTHING TO DO
}

void Texture::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    DASSERT(get(context).pipelineBindContext);
    auto& bindContext = *get(context).pipelineBindContext;

    glActiveTexture(GL_TEXTURE0 + bindingId);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

}    //  namespace renderer::ogl
