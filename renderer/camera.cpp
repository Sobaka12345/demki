#include "camera.hpp"

Camera::Camera(IShaderResourceProvider &provider)
    : m_viewProjection(
          provider.fetchHandle(ShaderBlockType::UNIFORM_DYNAMIC, m_viewProjection.s_layoutSize))
{
    m_descriptors[0].handle = m_viewProjection.handle();
    m_descriptors[0].binding = s_layout[0];
}

void Camera::setView(glm::mat4 view)
{
    auto old = m_viewProjection.get();
    old.view = std::move(view);
    m_viewProjection.set(old);
}

void Camera::setProjection(glm::mat4 projection)
{
    auto old = m_viewProjection.get();
    old.projection = std::move(projection);
    m_viewProjection.set(old);
}

void Camera::setViewProjection(ViewProjection viewProjection)
{
    m_viewProjection.set(std::move(viewProjection));
}

void Camera::bind(OperationContext &context)
{
    //    if (m_bindContext.expired())
    //    {
    //        m_bindContext = context.pipeline().bindContext(*this);
    //    }

    //    m_bindContext.lock()->bind(context, *this);

    context.pipeline().bindContext(*this).lock()->bind(context, *this);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Camera::uniforms() const
{
    return m_descriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Camera::dynamicUniforms() const
{
    return m_descriptors;
}
