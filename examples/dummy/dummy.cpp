#include "dummy.hpp"

#include "glsl_types.hpp"

#include "shaders_hpp/shader.vert.spv.hpp"
#include "shaders_hpp/shader.comp.spv.hpp"
#include "shaders_hpp/shader.frag.spv.hpp"

#include <camera.hpp>

#include <spirv_reflect.h>


using namespace renderer;

static std::span<const Vertex3DColoredTextured> cubeVertices()
{
    static const std::array<Vertex3DColoredTextured, 8> s_cubeVertices = {
        Vertex3DColoredTextured{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        Vertex3DColoredTextured{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        Vertex3DColoredTextured{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
        Vertex3DColoredTextured{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
        Vertex3DColoredTextured{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
        Vertex3DColoredTextured{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
        Vertex3DColoredTextured{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
        Vertex3DColoredTextured{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    };

    return s_cubeVertices;
}

static std::span<const IndexType> cubeIndices()
{
    static const std::array<IndexType, 36> s_cubeIndices = { 7, 6, 2, 2, 3, 7, 0, 4, 5, 5, 1, 0, 0, 2,
        6, 6, 4, 0, 7, 3, 1, 1, 5, 7, 3, 2, 0, 0, 1, 3, 4, 6, 7, 7, 5, 4 };

    return s_cubeIndices;
}

struct DrawPipelineObject : public IPipeline::Object
{
    std::shared_ptr<renderer::IStorageBuffer> vertices;
    std::shared_ptr<renderer::IStorageBuffer> indices;
    std::shared_ptr<renderer::IUniformBuffer> uniforms;

    using IPipeline::Object::Object;

    virtual void init() override {
        setBinding(0, indices);
        setBinding(1, vertices);
    }

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

    m_vertexBuffer = context().createStorageBuffer(
        IStorageBuffer::CreateInfo{}.size(cubeVertices().size_bytes()));
    m_vertexBuffer->write(cubeVertices().data(), cubeVertices().size_bytes());

    m_indexBuffer = context().createStorageBuffer(
        IStorageBuffer::CreateInfo{}.size(cubeIndices().size_bytes()));
    m_indexBuffer->write(cubeIndices().data(), cubeIndices().size_bytes());

    m_object = std::make_shared<DrawPipelineObject>(*m_renderPipeline);
    m_object->indices = m_indexBuffer;
    m_object->vertices = m_vertexBuffer;
    m_object->init();
    //auto uniformBuffer = context().createUniformBuffer(IUniformBuffer::CreateInfo{}.size(100));

    setFpsCap(60);
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
    m_object->bind(context);

    context.draw(cubeIndices().size());

    context.submit();
}
