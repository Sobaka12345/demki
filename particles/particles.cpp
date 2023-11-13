#include "particles.hpp"

#include "camera.hpp"
#include "particles.hpp"
#include "renderable.hpp"

ParticlesApplication::ParticlesApplication()
{
    m_timer.setIntervalMS(100);
    m_renderer = context().createRenderer({ .multisampling = context().maxSampleCount() });

    m_pipeline = context().createPipeline(
        IPipeline::CreateInfo{}
            .addInput(Vertex3DColoredTextured{})
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::VERTEX,
                .path = "./shaders/shader.vert.spv",
            })
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::FRAGMENT,
                .path = "./shaders/shader.frag.spv",
            })
            .addShaderInterfaceContainer<Camera>()
            .addShaderInterfaceContainer<Renderable>());

    m_camera = std::make_unique<Camera>(context().resources());
}

ParticlesApplication::~ParticlesApplication() {}

void ParticlesApplication::update(int64_t dt) {}

void ParticlesApplication::draw(IRenderTarget &renderTarget) {}
