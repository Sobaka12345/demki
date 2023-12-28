#include "texture.hpp"

namespace ogl {

Texture::Texture(GraphicsContext& context, CreateInfo createInfo) noexcept
    : m_context(context)
{}

Texture::~Texture() {}

}
