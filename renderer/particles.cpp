#include "particles.hpp"

#include <igraphics_context.hpp>

namespace renderer {

Particles::Particles(IGraphicsContext& context, std::span<const Particle> initialData)
    : m_currentIndex(0)
{
    const auto bufferInfo = IStorageBuffer::CreateInfo{ initialData };

    m_particlesBuffers[0] = context.createStorageBuffer(bufferInfo);
    m_particlesBuffers[1] = context.createStorageBuffer(bufferInfo);

    resource(0) = m_particlesBuffers[0];
    resource(1) = m_particlesBuffers[1];
}

void Particles::draw(OperationContext& context)
{
    m_particlesBuffers[m_currentIndex]->draw(context);
}

void Particles::accept(ComputerInfoVisitor& visitor) const
{
    m_particlesBuffers[m_currentIndex]->accept(visitor);
}

bool Particles::prepare(OperationContext& context)
{
    context.setOperationTarget(*this);
    return m_particlesBuffers[m_currentIndex]->prepare(context);
}

void Particles::present(OperationContext& context)
{
    m_particlesBuffers[m_currentIndex]->present(context);

    m_currentIndex = (m_currentIndex + 1) % m_particlesBuffers.size();
    std::swap(resource(1), resource(0));
}

}    //  namespace renderer
