#include "dummy.hpp"

#include "shaders_hpp/shader.vert.spv.hpp"
#include "shaders_hpp/shader.comp.spv.hpp"
#include "shaders_hpp/shader.frag.spv.hpp"

#include <camera.hpp>

#include <spirv_reflect.h>


using namespace renderer;

static constexpr std::array<Vertex3DColoredTextured, 8> s_cubeVertices = {
    Vertex3DColoredTextured{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    Vertex3DColoredTextured{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
};

static constexpr std::array<uint32_t, 36> s_cubeIndices = { 7, 6, 2, 2, 3, 7, 0, 4, 5, 5, 1, 0, 0,
    2, 6, 6, 4, 0, 7, 3, 1, 1, 5, 7, 3, 2, 0, 0, 1, 3, 4, 6, 7, 7, 5, 4 };

struct MultiUniformBuffer : public IShaderResource
{
    void bind(OperationContext& context, uint32_t bindingId) const {}

    std::vector<std::shared_ptr<IUniformBuffer>> m_buffers;
};

struct DrawPipelineDescriptor : public IPipeline::Descriptor
{
    std::shared_ptr<renderer::IStorageBuffer> drawCommands;
    std::shared_ptr<renderer::IUniformBuffer> uniforms;

    virtual std::span<const IShaderResource*> binding(uint32_t id) const override {}
};

struct RenderGroup
{};

Dummy::Dummy(int& argc, char** argv)
    : GraphicalApplication(argc, argv)
{
    m_renderer = context().createRenderer(IRenderer::CreateInfo{}
            .multisampling(Multisampling::MSA_4X)
            .clearValue(glm::vec4{ 0, 0, 0, 0 }));
    m_renderPipeline = context().createGraphicsPipeline(IGraphicsPipeline::CreateInfo{}
            .addShaderModule(shader_vert_spv)
            .addShaderModule(shader_frag_spv));

    auto buffer = context().createStorageBuffer(IStorageBuffer::CreateInfo{}.size(500));
    auto uniformBuffer = context().createUniformBuffer(IUniformBuffer::CreateInfo{}.size(100));
}

Dummy::~Dummy() {}

void Dummy::update(int64_t dt) {}

void Dummy::perform()
{
    auto context = m_renderer->start(window());
    context.setViewport({
        .x = 0,
        .y = 0,
        .width = static_cast<float>(window().width()),
        .height = static_cast<float>(window().height()),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });

    context.setScissors({
        .x = 0,
        .y = 0,
        .width = window().width(),
        .height = window().height(),
    });

    m_renderPipeline->bind(context);

    context.submit();
}
