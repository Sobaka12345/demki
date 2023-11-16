#include "particles_application.hpp"

#include "camera.hpp"
#include "particles.hpp"
#include "renderable.hpp"

ParticlesApplication::ParticlesApplication()
{
    m_timer.setIntervalMS(100);
    m_renderer = context().createRenderer({ .multisampling = context().maxSampleCount() });
    m_computer = context().createComputer({});

    m_pipeline = context().createPipeline(
        IPipeline::CreateInfo{}
            .type(IPipeline::COMPUTE)
            .addInput(Vertex3DColoredTextured{})
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::COMPUTE,
                .path = "./shaders/shader.compute.spv",
            })
            .addShaderInterfaceContainer<Particles>());
}

ParticlesApplication::~ParticlesApplication() {}

void ParticlesApplication::update(int64_t dt) {}

void ParticlesApplication::perform() {}
