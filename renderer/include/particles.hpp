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
    : public IComputeTarget
{
public:
    Particles(IGraphicsContext& context, std::span<const Particle> initialData);

    void draw(OperationContext& context);

    virtual bool prepare(OperationContext& context) override;
    virtual void present(OperationContext& context) override;

private:
    size_t m_currentIndex;
    std::array<std::shared_ptr<IStorageBuffer>, 2> m_particlesBuffers;
};

}    //  namespace renderer
