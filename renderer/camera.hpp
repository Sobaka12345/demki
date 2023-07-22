#pragma once

#include "ipipeline.hpp"
#include "uniform.hpp"
#include "uniform_value.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <array>

struct ViewProjection
{
    glm::mat4 view;
    glm::mat4 projection;
};

class Camera : public SIUniformContainer<Camera>
{
public:
    static constexpr std::array<UniformBinding, 1> s_layout = { UniformBinding{
        .id = UniformID::CAMERA,
        .type = UniformType::DYNAMIC,
        .stage = UniformStage::VERTEX,
    } };

public:
    Camera(IUniformProvider& provider);

    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);
    void setViewProjection(ViewProjection viewProjection);

    virtual void bind(RenderContext& context) override;
    virtual std::span<const UniformDescriptor> uniforms() const override;
    virtual std::span<const UniformDescriptor> dynamicUniforms() const override;

private:
    std::shared_ptr<IPipeline::IBindContext> m_bindContext;
    UniformValue<ViewProjection> m_viewProjection;
    std::array<UniformDescriptor, s_layout.size()> m_descriptors;
};
