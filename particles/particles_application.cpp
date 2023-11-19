#include "particles_application.hpp"

#include "camera.hpp"
#include "particles.hpp"
#include "renderable.hpp"

#include <icompute_pipeline.hpp>
#include <igraphics_pipeline.hpp>

#include <random>

struct DeltaTime : public SIShaderInterfaceContainer<DeltaTime>
{
public:
    static constexpr ShaderInterfaceLayout<1> s_layout = {
        ShaderInterfaceBinding{
            .id = InterfaceBlockID::IBLOCK_ID_0,
            .type = ShaderBlockType::UNIFORM_DYNAMIC,
            .stage = ShaderStage::COMPUTE,
        },
    };

public:
    DeltaTime(IShaderResourceProvider& provider)
        : m_value(provider.fetchHandle(s_layout[0].type, sizeof(float)))
    {
        m_descriptors[0].binding = s_layout[0];
        m_descriptors[0].handle = m_value.handle();
    }

    virtual void bind(OperationContext& context) override
    {
        context.pipeline().bindContext(*this).lock()->bind(context, *this);
    }

    void set(float value) { m_value.set(value); }

    virtual std::span<const InterfaceDescriptor> uniforms() const override { return m_descriptors; }

    virtual std::span<const InterfaceDescriptor> dynamicUniforms() const override
    {
        return m_descriptors;
    }

    UniformValue<float> m_value;
    std::array<InterfaceDescriptor, s_layout.size()> m_descriptors;
};

static uint64_t s_particleCount = 4096;

ParticlesApplication::ParticlesApplication()
{
    m_timer.setIntervalMS(100);
    m_renderer = context().createRenderer({ .multisampling = context().maxSampleCount() });
    m_computer = context().createComputer({});

    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    std::vector<Particle> particles(s_particleCount);
    for (auto& particle : particles)
    {
        float r = 0.25f * sqrt(rndDist(rndEngine));
        float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
        float x = r * cos(theta) * m_swapchain->height() / m_swapchain->width();
        float y = r * sin(theta);
        particle.pos = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
        particle.color =
            glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
    }

    m_particles = std::make_unique<Particles>(context(), particles);
    m_deltaTime = std::make_unique<DeltaTime>(context().resources());

    m_computePipeline = context().createComputePipeline(
        IComputePipeline::CreateInfo{}
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::COMPUTE,
                .path = "./shaders/shader.comp.spv",
            })
            .addShaderInterfaceContainer<DeltaTime>()
            .addShaderInterfaceContainer<Particles>()
            .setComputeDimensions({ .x = 256 }));

    m_graphicsPipeline = context().createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo{}
            .primitiveTopology(IGraphicsPipeline::CreateInfo::POINT)
            .addInput<Particle>()
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::VERTEX, .path = "./shaders/shader.vert.spv" })
            .addShader(IPipeline::ShaderInfo{
                .type = IPipeline::ShaderType::FRAGMENT, .path = "./shaders/shader.frag.spv" }));
}

ParticlesApplication::~ParticlesApplication() {}

void ParticlesApplication::update(int64_t dt)
{
    m_deltaTime->set(static_cast<glm::float32>(dt / 1000));
}

void ParticlesApplication::perform()
{
    auto computeContext = m_computer->start(*m_particles);
    m_computePipeline->bind(computeContext);
    m_deltaTime->bind(computeContext);
    m_particles->bind(computeContext);


    auto renderContext = m_renderer->start(*m_swapchain);
    renderContext.setViewport({
        .x = 0,
        .y = 0,
        .width = static_cast<float>(m_swapchain->width()),
        .height = static_cast<float>(m_swapchain->height()),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });

    renderContext.setScissors({
        .x = 0,
        .y = 0,
        .width = m_swapchain->width(),
        .height = m_swapchain->height(),
    });

    m_graphicsPipeline->bind(renderContext);
    m_particles->draw(renderContext);

    renderContext.waitForOperation(computeContext);

    computeContext.submit();
    renderContext.submit();
}
