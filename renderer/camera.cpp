#include "camera.hpp"

#include "igraphics_context.hpp"

namespace renderer {

Camera::Camera(IGraphicsContext& context)
    : m_viewProjection(context.createUniformValue<ViewProjection>())
{
    descriptor(0).resource = m_viewProjection;
}

void Camera::setView(glm::mat4 view)
{
    ViewProjection old = viewProjection();
    old.view = std::move(view);
    setViewProjection(old);
}

void Camera::setProjection(glm::mat4 projection)
{
    ViewProjection old = viewProjection();
    old.projection = std::move(projection);
    setViewProjection(old);
}

void Camera::setViewProjection(ViewProjection viewProjection)
{
    m_viewProjection.set(viewProjection);
}

ViewProjection Camera::viewProjection() const
{
    return m_viewProjection.get();
}

void Camera::bind(OperationContext& context)
{
    IShaderInterfaceContainer::bind(context);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Camera::uniforms() const
{
    return m_descriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Camera::dynamicUniforms() const
{
    return m_descriptors;
}

}    //  namespace renderer
