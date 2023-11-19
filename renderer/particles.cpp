#include "particles.hpp"

#include <igraphics_context.hpp>

Particles::Particles(const IGraphicsContext& context, std::span<const Particle> initialData)
{
    IStorageBuffer::CreateInfo bufferInfo{
        .elementLayoutSize = sizeof(Particle),
        .size = initialData.size_bytes(),
        .initialData = initialData.data(),
    };

    m_particlesIn = context.createStorageBuffer(bufferInfo);
    m_particlesOut = context.createStorageBuffer(bufferInfo);

    m_descriptors[0].binding = s_layout[0];
    m_descriptors[0].handle = m_particlesIn->handle();

    m_descriptors[1].binding = s_layout[1];
    m_descriptors[1].handle = m_particlesOut->handle();
}

void Particles::draw(OperationContext& context)
{
    m_particlesOut->bind(context);
    m_particlesOut->draw(context);
}

void Particles::bind(OperationContext& context)
{
    context.pipeline().bindContext(*this).lock()->bind(context, *this);
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::uniforms() const
{
    return m_descriptors;
}

std::span<const IShaderInterfaceContainer::InterfaceDescriptor> Particles::dynamicUniforms() const
{
    return {};
}

void Particles::accept(ComputerInfoVisitor& visitor) const
{
    m_particlesOut->accept(visitor);
}

bool Particles::prepare(OperationContext& context)
{
    return m_particlesOut->prepare(context);
}

void Particles::compute(OperationContext& context)
{
    return m_particlesOut->compute(context);
}
