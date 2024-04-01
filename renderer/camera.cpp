#include "camera.hpp"

namespace renderer {

Camera::Camera(IShaderResourceProvider& provider)
    : m_viewProjection(
          provider.fetchHandle(ShaderBlockType::UNIFORM_DYNAMIC, m_viewProjection.s_layoutSize))
{
    m_descriptors[0].handle = m_viewProjection.handle();
    m_descriptors[0].binding = s_layout[0];
}

void Camera::setView(glm::mat4 view)
{
    ViewProjection old = m_viewProjection.get();
    old.view = std::move(view);
    m_viewProjection.set(old);
}

void Camera::setProjection(glm::mat4 projection)
{
    ViewProjection old = m_viewProjection.get();
    old.projection = std::move(projection);
    m_viewProjection.set(old);
}

void Camera::setViewProjection(ViewProjection viewProjection)
{
    m_viewProjection.set(std::move(viewProjection));
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
