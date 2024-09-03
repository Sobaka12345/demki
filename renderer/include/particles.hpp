#pragma once

#include <icompute_target.hpp>
#include <ipipeline.hpp>

#include <ishader_interface_container.hpp>

namespace renderer {

class IStorageBuffer;
class IGraphicsContext;

struct Particle
{
    glm::vec2 pos;
    glm::vec2 velocity;
    glm::vec4 color;
};

class Particles
    : public ShaderInterfaceContainer<IShaderInterfaceContainer,
          ShaderInterfaceBindingMeta<Particle, ShaderBlockType::STORAGE, ShaderStage::COMPUTE>,
          ShaderInterfaceBindingMeta<Particle, ShaderBlockType::STORAGE, ShaderStage::COMPUTE>>
    , public IComputeTarget
{
public:
    Particles(IGraphicsContext& context, std::span<const Particle> initialData);

    void draw(OperationContext& context);

    virtual void accept(ComputerInfoVisitor& visitor) const override;
    virtual bool prepare(OperationContext& context) override;
    virtual void present(OperationContext& context) override;

private:
    size_t m_currentIndex;
    std::weak_ptr<IPipelineBindContext> m_bindContext;
    std::array<std::shared_ptr<IStorageBuffer>, 2> m_particlesBuffers;
    std::array<std::shared_ptr<IShaderResource>, 2> m_resourcesReverse;
};

}    //  namespace renderer
