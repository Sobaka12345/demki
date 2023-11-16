#include "particles.hpp"

Particles::Particles(IShaderResourceProvider &provider)
    : m_particlesBeforeArray(
          provider.fetchHandle(ShaderBlockType::STORAGE, m_particlesBeforeArray.s_layoutSize))
    , m_particlesNowArray(
          provider.fetchHandle(ShaderBlockType::STORAGE, m_particlesNowArray.s_layoutSize))
{
    m_descriptors[0].binding = s_layout[0];
    m_descriptors[0].handle = m_particlesBeforeArray.handle();
    m_descriptors[1].binding = s_layout[1];
    m_descriptors[1].handle = m_particlesNowArray.handle();
}

void Particles::setParticles(glm::mat4 view) {}

void Particles::bind(OperationContext &context) {}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::uniforms() const
{
    return m_descriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::dynamicUniforms() const
{
    return m_descriptors;
}
