#include "particles.hpp"

#include <igraphics_context.hpp>

namespace renderer {

Particles::Particles(IGraphicsContext& context, std::span<const Particle> initialData)
    : m_currentIndex(0)
{
    const auto bufferInfo = IStorageBuffer::CreateInfo{ initialData };

    m_particlesBuffers[0] = context.createStorageBuffer(bufferInfo);
    m_particlesBuffers[1] = context.createStorageBuffer(bufferInfo);

    descriptor(0).resource = m_particlesBuffers[0];
    descriptor(1).resource = m_particlesBuffers[1];

    m_descriptorsReverse[0] = descriptor(1);
    m_descriptorsReverse[1] = descriptor(0);
}

void Particles::draw(OperationContext& context)
{
    m_particlesBuffers[m_currentIndex]->bind(context);
    m_particlesBuffers[m_currentIndex]->draw(context);
}

void Particles::bind(OperationContext& context)
{
    IShaderInterfaceContainer::bind(context);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::uniforms() const
{
    if (m_currentIndex == 1) return m_descriptors;
    return m_descriptorsReverse;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::dynamicUniforms() const
{
    return {};
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
}

}    //  namespace renderer
