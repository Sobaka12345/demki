#include "dummy.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <camera.hpp>
#include <imodel.hpp>
#include <renderable.hpp>

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

Dummy::Dummy(int& argc, char** argv)
    : QtApplication(argc, argv)
{
    m_timer.setIntervalMS(50);
    m_renderer = context().createRenderer({ .multisampling = context().maxSampleCount() });

    m_pipeline = context().createGraphicsPipeline(
        IGraphicsPipeline::CreateInfo{}
            .addInput<Vertex3DColoredTextured>()
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

    m_camera = std::make_unique<Camera>(context());

    ViewProjection viewProjection;
    viewProjection.view = glm::lookAt(
        glm::vec3(0.0f, 3.0f, -4.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    viewProjection.projection = glm::perspective(
        glm::radians(45.0f), window().width() / static_cast<float>(window().height()), 0.1f, 8.0f);

    m_camera->setViewProjection(viewProjection);

    m_model = context().createModel(executablePath() / "models" / "viking_room.obj");
    m_texture = context().createTexture(executablePath() / "textures" / "viking_room.png");

    m_renderable = std::make_unique<Renderable>(context());
    m_renderable->setModel(m_model);
    m_renderable->setTexture(m_texture);
    m_renderable->setPosition(
        glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f)));
}

Dummy::~Dummy() {}

void Dummy::update(int64_t dt)
{
    if (m_timer.timePassed(dt))
    {
        m_renderable->setPosition(
            glm::rotate(m_renderable->position(), 0.01f, glm::vec3(0.f, 1.f, 0.f)));
    }
}

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

    m_pipeline->bind(context);

    m_camera->bind(context);

    m_renderable->bind(context);
    m_renderable->draw(context);

    context.submit();
}
