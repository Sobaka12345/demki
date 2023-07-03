#include "resource_manager.hpp"

#include "handles/device.hpp"

#include "../model.hpp"

#include <tiny_obj_loader.h>

#include <memory>
#include <unordered_map>

namespace vk {

using namespace handles;

ResourceManager::ResourceManager(const Device& device)
	: m_device(device)
	, m_defaultBufferSize(1024 * 1024)
{}

std::shared_ptr<Model> ResourceManager::loadModel(std::span<const Vertex3DColoredTextured> vertices,
    std::span<const uint32_t> indices)
{
    constexpr size_t vertexSize = sizeof(vertices[0]);
    constexpr size_t indexSize = sizeof(indices[0]);

    const size_t verticesSize = vertices.size() * vertexSize;
    const size_t indicesSize = indices.size() * indexSize;

    m_modelBuffers.emplace_back(m_device,
        BufferCreateInfo()
            .size(verticesSize)
            .usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_modelBuffers.back().allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_indBuffers.emplace_back(m_device,
        BufferCreateInfo()
            .size(indicesSize)
            .usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_indBuffers.back().allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Buffer stagingBuffer(m_device, Buffer::staging().size(indicesSize + verticesSize));
    auto mapped =
        stagingBuffer
            .allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .lock()
            ->map();
    mapped.lock()->write(vertices.data(), verticesSize);
    mapped.lock()->write(indices.data(), indicesSize, verticesSize);

    stagingBuffer.memory().lock()->unmap();

    stagingBuffer.copyTo(m_modelBuffers.back(), bufferCopy(verticesSize));
    stagingBuffer.copyTo(m_indBuffers.back(), bufferCopy(indicesSize, verticesSize));

    return std::make_shared<Model>(Model::Descriptor{
        .vertices =
            Buffer::Descriptor{
                .buffer = m_modelBuffers.back(),
                .offset = 0,
                .range = verticesSize,
                .objectSize = vertexSize,
            },
        .indices =
            Buffer::Descriptor{
                .buffer = m_indBuffers.back(),
                .offset = 0,
                .range = indicesSize,
                .objectSize = indexSize,
            },
    });
}

std::shared_ptr<Model> ResourceManager::loadModel(std::filesystem::path path,
    Resource::Storage targetStorage)
{
    std::vector<Vertex3DColoredTextured> vertices;
    std::vector<uint32_t> indices;

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

    return loadModel(vertices, indices);
}

}    //  namespace vk
