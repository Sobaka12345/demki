#include "renderable.hpp"

#include "igraphics_context.hpp"

#include <imesh.hpp>
#include <itexture.hpp>
#include <ipipeline.hpp>

namespace renderer {

Renderable::Renderable(IGraphicsContext& context)

{
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

    //IShaderInterfaceContainer::bind(context);
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

void Renderable::setTexture(std::weak_ptr<ITexture> value)
{
    m_texture = value;
}

void Renderable::setPosition(glm::mat4 position)
{
}

glm::mat4 Renderable::position() const
{
    return {};
}

}    //  namespace renderer
