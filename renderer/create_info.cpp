#include "imodel.hpp"
#include "itexture.hpp"

#include "assert.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <tiny_obj_loader.h>

namespace renderer {

IModel::CreateInfo::CreateInfo(std::filesystem::path path)
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

IModel::CreateInfo::CreateInfo(std::span<const Vertex3DColoredTextured> vertices,
    std::span<const uint32_t> indices)
    : vertices(vertices.begin(), vertices.end())
    , indices(indices.begin(), indices.end())
{}

ITexture::CreateInfo::CreateInfo(std::filesystem::path path)
{
    pixels = stbi_load(path.string().c_str(), &width, &height, &textureChannels, STBI_rgb_alpha);
    imageSize = width * height * 4;
}

ITexture::CreateInfo::CreateInfo(CreateInfo&& other)
    : pixels(other.pixels)
    , imageSize(other.imageSize)
    , textureChannels(other.textureChannels)
    , width(other.width)
    , height(other.height)
{
    other.pixels = nullptr;
}

ITexture::CreateInfo::~CreateInfo()
{
    if (pixels)
    {
        stbi_image_free(pixels);
    }
}

}    //  namespace renderer
