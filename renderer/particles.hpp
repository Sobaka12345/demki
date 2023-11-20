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
    glm::vec4 color;
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

    void draw(OperationContext& context);

    virtual void bind(OperationContext& context) override;
    virtual std::span<const InterfaceDescriptor> uniforms() const override;
    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override;

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(OperationContext& context) override;
    virtual void compute(OperationContext& context) override;

private:
    size_t m_currentIndex;
    std::weak_ptr<IPipeline::IBindContext> m_bindContext;
    std::array<std::shared_ptr<IStorageBuffer>, 2> m_particlesBuffers;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptorsReverse;
};
