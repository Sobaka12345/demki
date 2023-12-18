#include "cubic.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <camera.hpp>
#include <imodel.hpp>
#include <renderable.hpp>
#include <utils.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>

static constexpr float s_visibleRange = 50.f;
static constexpr glm::vec3 s_startPosition = { 0.0f, 0.0f, 4.0f };

static constexpr glm::vec<3, size_t> s_mapSize = { 10, 10, 10 };

struct BoundingBox
{
    glm::vec3 center;
    glm::vec3 lengths;

    float x0() const { return center.x - lengths.x / 2.f; }

    float x1() const { return center.x + lengths.x / 2.f; }

    float y0() const { return center.y - lengths.y / 2.f; }

    float y1() const { return center.y + lengths.y / 2.f; }

    float z0() const { return center.z - lengths.z / 2.f; }

    float z1() const { return center.z + lengths.z / 2.f; }

    bool hasIntersection(const glm::vec3& point)
    {
        return point.x <= x1() && point.x >= x0() || point.y <= y1() && point.y >= y0() ||
            point.z <= z1() && point.z >= z0();
    }

    bool hasIntersection(const BoundingBox& other)
    {
        return (x0() <= other.x1() && x1() >= other.x0()) &&
            (y0() <= other.y1() && y1() >= other.y0()) &&
            (z0() <= other.z1() && z1() >= other.z0());
    }
};

class Block : public Renderable
{
public:
    using Renderable::Renderable;

    BoundingBox boundingBox() const
    {
        return { .center = position()[3], .lengths = { 1.f, 1.f, 1.f } };
    }
};

struct Map
    : public std::array<std::array<std::array<std::optional<Block>, s_mapSize.z>, s_mapSize.y>,
          s_mapSize.x>
{
    glm::vec3 center = { 0.0f, 0.0f, 0.0f };

    glm::vec<3, size_t> toBlockIndices(glm::vec3 pos)
    {
        if (pos.x >= (center.x + size() / 2) || (pos.x < center.x - (size() / 2)) ||
            pos.y >= (center.y + front().size() / 2) || pos.y < (center.y - (front().size() / 2)) ||
            pos.z >= (center.z + front().front().size() / 2) ||
            pos.z < (center.z - (front().front().size() / 2)))
        {
            return { -1, -1, -1 };
        }

        glm::vec<3, size_t> result;

        result.x = center.x + size() / 2 + pos.x;
        result.y = center.y + front().size() / 2 + pos.y;
        result.z = center.z + front().front().size() / 2 + pos.z;

        return result;
    }

    glm::vec3 toBlockPosition(glm::vec<3, size_t> indices)
    {
        constexpr float blockHalf = 0.5f;
        return glm::vec3(center.x - size() / 2 + indices.x + blockHalf,
            center.y - front().size() / 2 + indices.y + blockHalf,
            center.z - front().front().size() / 2 + indices.z + blockHalf);
    }

    void draw(OperationContext& context)
    {
        for (size_t x = 0; x < size(); ++x)
            for (size_t y = 0; y < (*this)[x].size(); ++y)
                for (size_t z = 0; z < (*this)[x][y].size(); ++z)
                {
                    if (auto& block = (*this)[x][y].at(z); block.has_value())
                    {
                        block->bind(context);
                        block->draw(context);
                    }
                }
    }
};

enum Movement
{
    NONE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    STRAFE_LEFT = 4,
    STRAFE_RIGHT = 8,
    FALL = 16,
    JUMP = 32,
};

class Hero : public Renderable
{
    static constexpr float s_velocity = 0.1f;

public:
    Hero(GraphicalApplication& app, Map& map)
        : Renderable(app.context().resources())
        , m_app(app)
        , m_map(map)
        , m_camera(app.context().resources())
        , m_up(0.0f, -1.0f, 0.0f)
        , m_currentPosition(s_startPosition)
    {
        ViewProjection vp = m_camera.viewProjection();

        m_direction = glm::normalize(map.center - m_currentPosition);
        vp.view = glm::lookAt(m_currentPosition, m_currentPosition + m_direction, m_up);
        vp.projection = glm::perspective(glm::radians(45.0f),
            app.clientWidth() / static_cast<float>(app.clientHeight()), 0.1f, s_visibleRange);
        m_camera.setViewProjection(vp);
    }

    virtual void bind(OperationContext& context) override
    {
        m_camera.bind(context);
        Renderable::bind(context);
    }

    void move(Movement m)
    {
        glm::vec3 initialPos = m_currentPosition;

        if (m == 0) return;

        float dirFB = 0.f;
        float dirSLR = 0.f;

        if (m & FORWARD) dirFB += 1;
        if (m & BACKWARD) dirFB -= 1;
        if (m & STRAFE_LEFT) dirSLR -= 1;
        if (m & STRAFE_RIGHT) dirSLR += 1;

        constexpr glm::vec3 up(0.f, 1.f, 0.f);
        constexpr glm::vec3 down(0.f, -1.f, 0.f);

        glm::vec3 direction = glm::cross(m_direction, m_up) * dirSLR + m_direction * dirFB +
            down * (m & FALL ? 1.f : 0.f) + up * (m & JUMP ? 2.5f : 0.f);
        if (glm::length(direction) == 0) return;
        direction = glm::normalize(direction);

        glm::vec<3, int> moveAxis{ 1, 1, 1 };
        for (auto& x : m_map)
            for (auto& y : x)
                for (auto& z : y)
                {
                    if (!z.has_value()) continue;
                    for (size_t i = 0; i < m_direction.length(); ++i)
                    {
                        if (moveAxis[i] == 0) continue;

                        glm::vec3 shift{ 0.f, 0.f, 0.f };
                        shift[i] = direction[i];

                        m_currentPosition = initialPos + shift * s_velocity;

                        auto bb1 = z->boundingBox();
                        auto bb2 = boundingBox();

                        if (bb1.hasIntersection(bb2))
                        {
                            moveAxis[i] = 0;
                        }
                    }
                }

        m_currentPosition.x = initialPos.x + moveAxis.x * direction.x * s_velocity;
        m_currentPosition.y = initialPos.y + moveAxis.y * direction.y * s_velocity;
        m_currentPosition.z = initialPos.z + moveAxis.z * direction.z * s_velocity;

        m_camera.setView(glm::lookAt(m_currentPosition, m_currentPosition + m_direction, m_up));
    }

    void rotate(float diffX, float diffY)
    {
        constexpr float sensitivity = 0.1f;

        float kekX = diffX * sensitivity;
        float kekY = diffY * sensitivity;

        m_yaw += kekX;
        m_pitch += kekY;

        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_direction = glm::normalize(direction);

        constexpr glm::vec3 up(0.f, -1.f, 0.f);
        glm::vec3 rightVector = glm::cross(up, m_direction);
        m_up = glm::normalize(glm::cross(m_direction, rightVector));

        m_camera.setView(glm::lookAt(m_currentPosition, m_currentPosition + m_direction, m_up));
    }

    void buildBlock()
    {
        constexpr float maxRange = 4.f;

        for (float i = 2.f; i <= maxRange; i += 0.05f)
        {
            glm::vec<3, size_t> idx = m_map.toBlockIndices(m_currentPosition + m_direction * i);
            if (idx.x == -1) continue;

            if (auto block = m_map[idx.x][idx.y].at(idx.z); block.has_value())
            {
                float j = i;
                while (j > 2.f && idx.x != -1 && m_map[idx.x][idx.y][idx.z].has_value())
                {
                    j -= 0.05f;
                    idx = m_map.toBlockIndices(m_currentPosition + m_direction * j);
                }

                if (idx.x != -1 && !m_map[idx.x][idx.y][idx.z].has_value())
                {
                    auto& newBlock = m_map[idx.x][idx.y][idx.z];
                    newBlock.emplace(m_app.context().resources());
                    newBlock.value().setModel(block->model());
                    newBlock.value().setTexture(block->texture());
                    newBlock.value().setPosition(
                        glm::translate(glm::identity<glm::mat4>(), m_map.toBlockPosition(idx)));
                    break;
                }
            }
        }
    }

    BoundingBox boundingBox() const
    {
        return { .center = m_currentPosition + glm::vec3{ 0.f, -0.5f, 0.f },
            .lengths = { 0.8f, 2.f, 0.8f } };
    }

private:
    const GraphicalApplication& m_app;
    Camera m_camera;
    Map& m_map;

    float m_yaw = -90.0f;
    float m_pitch = 0.f;

    glm::vec3 m_up;
    glm::vec3 m_direction;
    glm::vec3 m_currentPosition;
};

Cubic::Cubic(uint32_t windowWidth, uint32_t windowHeight)
    : GraphicalApplication("Cubic", windowWidth, windowHeight)
    , m_movement(FALL)
    , xCursorPrev(0)
    , yCursorPrev(0)
{
    m_jumpTimer.setIntervalMS(300);
    m_placeBlockTimer.setIntervalMS(100);
    m_movementTimer.setIntervalMS(10);
    m_turnTimer.setIntervalMS(1);
    m_renderer = context().createRenderer({ .multisampling = context().maxSampleCount() });

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window().glfwHandle(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    //  glfwSetInputMode(window().glfwHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    window().registerOnKeyPressedCallback(std::bind(&Cubic::onKeyPressed, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        std::placeholders::_4));

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
            .addShaderInterfaceContainer<Renderable>(10));

    m_map = std::make_unique<Map>();
    m_hero = std::make_unique<Hero>(*this, *m_map);

    m_model = context().resources().createModel(executablePath() / "models" / "Erde mit Grass.obj");
    m_texture = context().resources().createTexture(
        { .path = executablePath() / "textures" / "Erde mit Grass.png" });

    for (size_t x = 0; x < m_map->size(); ++x)
        for (size_t y = 0; y < (*m_map)[x].size(); ++y)
            for (size_t z = 0; z < (*m_map)[x][y].size(); ++z)
            {
                auto heroBox = m_hero->boundingBox();
                auto idx = m_map->toBlockIndices({ heroBox.center.x,
                    int(heroBox.center.y - heroBox.lengths.y - 1.f), heroBox.center.z });

                if (y == idx.y)
                {
                    auto& block = (*m_map)[x][y][z];
                    block.emplace(context().resources());
                    block->setModel(m_model);
                    block->setTexture(m_texture);
                    block->setPosition(glm::translate(glm::identity<glm::mat4>(),
                        m_map->toBlockPosition({ x, y, z })));
                }
            }
}

Cubic::~Cubic() {}

void Cubic::onKeyPressed(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        m_movement |= FORWARD;
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        m_movement ^= FORWARD;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        m_movement |= BACKWARD;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        m_movement ^= BACKWARD;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        m_movement |= STRAFE_LEFT;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        m_movement ^= STRAFE_LEFT;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        m_movement |= STRAFE_RIGHT;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        m_movement ^= STRAFE_RIGHT;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        m_movement |= JUMP;
        m_jumpTimer.reset();
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE && m_movement & JUMP)
    {
        m_movement ^= JUMP;
    }
}

void Cubic::update(int64_t dt)
{
    if (m_movementTimer.timePassed(dt))
    {
        m_hero->move(Movement(m_movement));
    }
    if (m_turnTimer.timePassed(dt))
    {
        double xCursorCurrent, yCursorCurrent;
        glfwGetCursorPos(window().glfwHandle(), &xCursorCurrent, &yCursorCurrent);
        m_hero->rotate(xCursorPrev - xCursorCurrent, yCursorPrev - yCursorCurrent);
        xCursorPrev = xCursorCurrent;
        yCursorPrev = yCursorCurrent;
    }
    if (auto state = glfwGetMouseButton(window().glfwHandle(), GLFW_MOUSE_BUTTON_1);
        state == GLFW_PRESS && m_placeBlockTimer.timePassed(dt))
    {
        m_hero->buildBlock();
    }
    if (m_jumpTimer.timePassed(dt) && m_movement & JUMP)
    {
        m_movement ^= JUMP;
    }
}

void Cubic::perform()
{
    auto context = m_renderer->start(*m_swapchain);
    context.setViewport({
        .x = 0,
        .y = 0,
        .width = static_cast<float>(m_swapchain->width()),
        .height = static_cast<float>(m_swapchain->height()),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });

    context.setScissors({
        .x = 0,
        .y = 0,
        .width = m_swapchain->width(),
        .height = m_swapchain->height(),
    });

    m_pipeline->bind(context);

    m_hero->bind(context);
    m_map->draw(context);

    context.submit();
}
