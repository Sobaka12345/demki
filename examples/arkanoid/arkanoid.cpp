#include "arkanoid.hpp"

#include <camera.hpp>
#include <renderable.hpp>

using namespace renderer;

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
        return { .center = position()[3], .lengths = { 1.f, 0.5f, 1.f } };
    }
};

Arkanoid::Arkanoid(int& argc, char** argv)
    : engine::GraphicalApplication(argc, argv)
{
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

    for (int i = 0; i < 10; ++i)
    {
    }
}

Arkanoid::~Arkanoid() {}

void Arkanoid::update(int64_t dt) {}

void Arkanoid::perform() {}
