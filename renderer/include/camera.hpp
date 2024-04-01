#pragma once

#include "ipipeline.hpp"
#include "../uniform_value.hpp"

#include <ishader_interface.hpp>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <array>

namespace renderer {

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

class Camera : public SIShaderInterfaceContainer<Camera>
{
public:
    static constexpr ShaderInterfaceLayout<1> s_layout = { ShaderInterfaceBinding{
        .type = ShaderBlockType::UNIFORM_DYNAMIC,
        .stage = ShaderStage::VERTEX,
    } };

public:
    Camera(IShaderResourceProvider& provider);

    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);
    void setViewProjection(ViewProjection viewProjection);
    ViewProjection viewProjection() const;

    virtual void bind(OperationContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

private:
    std::weak_ptr<IPipelineBindContext> m_bindContext;
    UniformValue<ViewProjection> m_viewProjection;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
};

}    //  namespace renderer
