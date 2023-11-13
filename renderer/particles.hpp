#pragma once

#include "ipipeline.hpp"
#include "shader_interface.hpp"
#include "uniform_value.hpp"

class Particles : public SIShaderInterfaceContainer<Particles>
{
public:
    static constexpr ShaderInterfaceLayout<3> s_layout = {
        ShaderInterfaceBinding{
            .id = InterfaceBlockID::IBLOCK_ID_1,
            .type = ShaderBlockType::STORAGE,
            .stage = ShaderStage::COMPUTE,
        },
        ShaderInterfaceBinding{
            .id = InterfaceBlockID::IBLOCK_ID_2,
            .type = ShaderBlockType::STORAGE,
            .stage = ShaderStage::COMPUTE,
        }
    };

public:
    Particles(IShaderResourceProvider& provider);

    void setParticles(glm::mat4 view);

    virtual void bind(RenderContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

private:
    std::weak_ptr<IPipeline::IBindContext> m_bindContext;
    //  UniformValue<ViewProjection> m_viewProjection;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
};
