#include "particles.hpp"

#include <igraphics_context.hpp>

Particles::Particles(IGraphicsContext& context, std::span<const Particle> initialData)
    : m_currentIndex(0)
{
    const auto bufferInfo = IStorageBuffer::CreateInfo{ initialData };

    m_particlesBuffers[0] = context.createStorageBuffer(bufferInfo);
    m_particlesBuffers[1] = context.createStorageBuffer(bufferInfo);

    m_descriptors[0].binding = s_layout[0];
    m_descriptors[0].handle = m_particlesBuffers[0]->handle();

    m_descriptors[1].binding = s_layout[1];
    m_descriptors[1].handle = m_particlesBuffers[1]->handle();

    m_descriptorsReverse[0].binding = s_layout[0];
    m_descriptorsReverse[0].handle = m_descriptors[1].handle;

    m_descriptorsReverse[1].binding = s_layout[1];
    m_descriptorsReverse[1].handle = m_descriptors[0].handle;
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
