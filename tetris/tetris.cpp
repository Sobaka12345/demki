#include "tetris.hpp"

#include "field.hpp"

#include <camera.hpp>
#include <renderable.hpp>

#include <iostream>
#include <variant>

#include <GLFW/glfw3.h>

Tetris::Tetris()
    : m_dx(0)
    , m_rotate(false)
    , m_flushedTotal(0)
{
    m_gameTimer.setIntervalMS(500);
    m_moveTimer.setIntervalMS(150);
    m_rotationTimer.setIntervalMS(200);

    window().registerOnKeyPressedCallback(std::bind(&Tetris::onKeyPressed, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        std::placeholders::_4));

    {
        m_camera = std::make_shared<Camera>(context().resources());

        constexpr float fov = 45.0f;
        constexpr float distance = Field::s_height * 90.0f / fov;
        ViewProjection viewProjection;
        viewProjection.view = glm::lookAt(
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, distance),
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        viewProjection.projection = glm::perspective(glm::radians(fov),
            clientWidth() / static_cast<float>(clientHeight()),
            0.1f,
            distance + 1.0f);
        m_camera->setViewProjection(viewProjection);
    }

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
            .addUniformContainer<Camera>()
            .addUniformContainer<Renderable>());

    m_renderer = context().createRenderer({});

    m_field = std::make_shared<Field>(context().resources());
    m_field->flushRowsAndSpawnFigure();
}

Tetris::~Tetris() {}

void Tetris::onKeyPressed(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        m_rotate = true;
        m_field->tryRotateFigure();
        m_rotationTimer.reset(m_rotationTimer.interval());
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        m_rotate = false;
    }
    else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
    {
        m_dx += (key == GLFW_KEY_RIGHT) - (key == GLFW_KEY_LEFT);
        m_field->tryMoveFigure(m_dx, 0);
        m_moveTimer.reset();
    }
    else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && action == GLFW_RELEASE)
    {
        m_dx += -(key == GLFW_KEY_RIGHT) + (key == GLFW_KEY_LEFT);
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        m_gameTimer.setSpeedUp(2.0f);
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    {
        m_gameTimer.setNormalSpeed();
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        while (m_field->tryMoveFigure(0, 1))
            ;
    }
}

void Tetris::update(int64_t dt)
{
    if (m_rotate && m_rotationTimer.timePassed(dt))
    {
        m_field->tryRotateFigure();
    }
    if (m_dx != 0 && m_moveTimer.timePassed(dt))
    {
        m_field->tryMoveFigure(m_dx, 0);
    }

    if (m_gameTimer.timePassed(dt))
    {
        if (!m_field->tryMoveFigure(0, 1))
        {
            if (m_field->isBlocksOverflow())
            {
                std::cout << "YOU ARE A LOSER! TOTAL FLUSHED: " << m_flushedTotal << std::endl;
                window().close();
            }
            const int32_t flushed = m_field->flushRowsAndSpawnFigure();
            m_flushedTotal += flushed;
            std::cout << "FLUSHED ROWS COUNT: " << flushed << " TOTAL: " << m_flushedTotal
                      << std::endl;

            if (flushed && m_flushedTotal % 8 == 0)
            {
                m_gameTimer.setInterval(m_gameTimer.interval() * 0.8f);
            }
        }
    }
}

void Tetris::draw(IRenderTarget& target)
{
    auto context = m_renderer->start(target);
    context.setViewport({
        .x = 0,
        .y = 0,
        .width = static_cast<float>(clientWidth()),
        .height = static_cast<float>(clientHeight()),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });

    context.setScissors({
        .x = 0,
        .y = 0,
        .width = clientWidth(),
        .height = clientHeight(),
    });

    m_pipeline->bind(context);
    m_camera->bind(context);
    m_field->draw(context);
}
