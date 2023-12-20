#include "resource_manager.hpp"

#include "graphics_context.hpp"

#include "model.hpp"
#include "texture.hpp"

#include <ishader_interface_handle.hpp>

#include <tiny_obj_loader.h>

#include <memory>
#include <unordered_map>

namespace vk {

using namespace handles;

ResourceManager::ResourceManager(const GraphicsContext& context)
    : m_context(context)
    , m_defaultBufferSize(1024 * 1024)
{}

ResourceManager::~ResourceManager() {}

std::shared_ptr<IModel> ResourceManager::createModel(
    std::span<const Vertex3DColoredTextured> vertices, std::span<const uint32_t> indices)
{
    constexpr size_t vertexSize = sizeof(vertices[0]);
    constexpr size_t indexSize = sizeof(indices[0]);

    const size_t verticesSize = vertices.size() * vertexSize;
    const size_t indicesSize = indices.size() * indexSize;

    m_modelBuffers.emplace_back(m_context.device(),
        BufferCreateInfo()
            .size(verticesSize)
            .usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_modelBuffers.back().allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_indBuffers.emplace_back(m_context.device(),
        BufferCreateInfo()
            .size(indicesSize)
            .usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_indBuffers.back().allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Buffer stagingBuffer(m_context.device(), Buffer::staging().size(indicesSize + verticesSize));
    auto mapped =
        stagingBuffer
            .allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .lock()
            ->map();
    mapped.lock()->write(vertices.data(), verticesSize);
    mapped.lock()->write(indices.data(), indicesSize, verticesSize);

    stagingBuffer.memory().lock()->unmap();

    stagingBuffer.copyTo(m_modelBuffers.back(), BufferCopy{}.size(verticesSize));
    stagingBuffer.copyTo(m_indBuffers.back(),
        BufferCopy{}.size(indicesSize).srcOffset(verticesSize));

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

std::shared_ptr<IModel> ResourceManager::createModel(std::filesystem::path path)
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

    return createModel(vertices, indices);
}

std::shared_ptr<ITexture> ResourceManager::createTexture(ITexture::CreateInfo createInfo)
{
    return std::make_shared<Texture>(m_context, createInfo);
}

uint32_t ResourceManager::dynamicAlignment(uint32_t layoutSize) const
{
    const uint32_t minAlignment =
        m_context.device().physicalDeviceProperties().limits.minUniformBufferOffsetAlignment;

    if (minAlignment > 0)
    {
        layoutSize = (layoutSize + minAlignment - 1) & ~(minAlignment - 1);
    }
    return layoutSize;
}

std::shared_ptr<ShaderInterfaceHandle> ResourceManager::fetchHandleSpecific(ShaderBlockType sbt,
    uint32_t layoutSize)
{
    const uint32_t alignment = dynamicAlignment(layoutSize);

    auto insertAndFetchSpecificHandle = [&](auto& map) {
        if (auto el = map.find(layoutSize); el != map.end())
        {
            return ShaderInterfaceHandle::create(el->second);
        }
        using PairType = typename std::remove_reference<decltype(map)>::type::value_type;
        auto [iter, _] = map.emplace(PairType{
            alignment, typename PairType::second_type{ m_context.device(), alignment, 100 } });

        return ShaderInterfaceHandle::create(iter->second);
    };

    if (sbt == ShaderBlockType::STORAGE)
    {
        return insertAndFetchSpecificHandle(m_storageShaderResources);
    }
    else if (sbt == ShaderBlockType::UNIFORM_DYNAMIC)
    {
        return insertAndFetchSpecificHandle(m_dynamicUniformShaderResources);
    }

    return insertAndFetchSpecificHandle(m_staticUniformShaderResources);
}

std::shared_ptr<IShaderInterfaceHandle> ResourceManager::fetchHandle(ShaderBlockType sbt,
    uint32_t layoutSize)
{
    return fetchHandleSpecific(sbt, layoutSize);
}

}    //  namespace vk
