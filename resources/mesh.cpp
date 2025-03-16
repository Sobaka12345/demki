#include "mesh.hpp"

#include <tiny_obj_loader.h>

namespace resources {

Mesh::Mesh(std::filesystem::path path) noexcept
    : IResource(TYPE_MESH)
{
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
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

Mesh::Mesh(std::span<const Vertex3DColoredTextured> vertices,
    std::span<const uint32_t> indices) noexcept
    : IResource(TYPE_MESH)
    , indices(indices.begin(), indices.end())
    , vertices(vertices.begin(), vertices.end())
{}

size_t Mesh::sizeBytes() const
{
    return vertices.size() * sizeof(decltype(vertices)::value_type) +
        indices.size() * sizeof(decltype(indices)::value_type);
}

}    //  namespace resources
