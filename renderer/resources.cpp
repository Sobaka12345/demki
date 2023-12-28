#include "resources.hpp"
#include "assert.hpp"

#include <tiny_obj_loader.h>

Resources::Resources(std::filesystem::path root) noexcept
    : m_root(root)
{}

std::pair<std::vector<Vertex3DColoredTextured>, std::vector<uint32_t>> Resources::loadModelData(
    std::filesystem::path path) noexcept
{
    std::pair<std::vector<Vertex3DColoredTextured>, std::vector<uint32_t>> result;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    ASSERT(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str()),
        warn + err);

    std::unordered_map<Vertex3DColoredTextured, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex3DColoredTextured vertex{};

            vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2] };

            vertex.texture = { attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(result.first.size());
                result.first.push_back(vertex);
            }

            result.second.push_back(uniqueVertices[vertex]);
        }
    }

    return result;
}
