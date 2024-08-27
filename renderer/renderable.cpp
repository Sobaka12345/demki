#include "renderable.hpp"

#include "igraphics_context.hpp"

#include <imesh.hpp>
#include <itexture.hpp>
#include <ipipeline.hpp>

namespace renderer {

Renderable::Renderable(IGraphicsContext& context)
    : m_position(context.createUniformValue<glm::mat4>())
{
    descriptor(0).resource = m_position;
}

Renderable::~Renderable() {}

void Renderable::draw(OperationContext& context) const
{
    if (!m_mesh.expired())
    {
        m_mesh.lock()->draw(context);
    }
}

void Renderable::bind(OperationContext& context)
{
    if (m_mesh.expired()) return;

    m_mesh.lock()->bind(context);

    IShaderInterfaceContainer::bind(context);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Renderable::uniforms() const
{
    return m_descriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Renderable::dynamicUniforms() const
{
    return std::span{ m_descriptors.begin(), 1 };
}

std::weak_ptr<IMesh> Renderable::mesh() const
{
    return m_mesh;
}

void Renderable::setMesh(std::weak_ptr<IMesh> mesh)
{
    m_mesh = mesh;
}

std::weak_ptr<ITexture> Renderable::texture() const
{
    return m_texture;
}

void Renderable::setTexture(std::weak_ptr<ITexture> texture)
{
    if (!texture.expired())
    {
        m_texture = texture;
        descriptor(1).resource = m_texture.lock();
    }
}

void Renderable::setPosition(glm::mat4 position)
{
    m_position.set(position);
}

glm::mat4 Renderable::position() const
{
    return m_position.get();
}

}    //  namespace renderer
