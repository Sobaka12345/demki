#pragma once

#include "ipipeline.hpp"
#include "../include/uniform_value.hpp"
#include "../include/ishader_interface_container.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

namespace renderer {

class IGraphicsContext;

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

class Camera
    : public ShaderInterfaceContainer<IShaderInterfaceContainer,
          ShaderInterfaceBindingMeta<ViewProjection,
              ShaderBlockType::UNIFORM_DYNAMIC,
              ShaderStage::VERTEX>>
{
public:
    Camera(IGraphicsContext& provider);

    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);
    void setViewProjection(ViewProjection viewProjection);
    ViewProjection viewProjection() const;

private:
    UniformValue<ViewProjection> m_viewProjection;
};

}    //  namespace renderer
