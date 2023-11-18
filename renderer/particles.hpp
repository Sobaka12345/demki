#pragma once

#include "icompute_target.hpp"
#include "ipipeline.hpp"

#include "shader_interface.hpp"
#include "storage_buffer_value.hpp"

class IStorageBuffer;
class IGraphicsContext;

struct Particle
{
    glm::vec2 pos;
    glm::vec2 velocity;
    glm::vec3 color;
};

class Particles
    : public SIShaderInterfaceContainer<Particles>
    , public IComputeTarget
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
    Particles(const IGraphicsContext& context, std::span<const Particle> initialData);

    virtual void bind(OperationContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(OperationContext& context) override;
    virtual void compute(OperationContext& context) override;

private:
    std::weak_ptr<IPipeline::IBindContext> m_bindContext;
    std::shared_ptr<IStorageBuffer> m_particlesIn;
    std::shared_ptr<IStorageBuffer> m_particlesOut;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
};
