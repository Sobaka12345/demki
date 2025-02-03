#include "camera.hpp"

#include "igraphics_context.hpp"

namespace renderer {

Camera::Camera(IGraphicsContext& context)
{
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

}

ViewProjection Camera::viewProjection() const
{
    return {};
}

}    //  namespace renderer
