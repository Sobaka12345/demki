#pragma once

#include "ipipeline.hpp"
#include "shader_interface.hpp"
#include "storage_buffer_value.hpp"

struct Particle
{
    glm::vec2 pos;
    glm::vec2 velocity;
    glm::vec3 color;
};

class Particles : public SIShaderInterfaceContainer<Particles>
{
public:
    static constexpr ShaderInterfaceLayout<2> s_layout = {
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
    StorageBufferValue<Particle, 256> m_particlesBeforeArray;
    StorageBufferValue<Particle, 256> m_particlesNowArray;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
};
