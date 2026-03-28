#include "resources.hpp"

#include <limits>

namespace resources 
{


template<>
Resources<Vk>::Resources(gapi::GApiContext<Vk>& ctx, std::filesystem::path rootPath) noexcept
    : ResourcesBase(std::move(rootPath))
{
}

template <>
MeshView Resources<Vk>::mapToGPU(ResourcesBase::MeshHandle handle) noexcept
{
    MeshView result {};
    const MeshData& mesh = *handle;

    const auto vertexCount = mesh.vertices.size();
    const auto indexCount = mesh.indices.size();

    const uint64_t vertexBytes = static_cast<uint64_t>(vertexCount) * sizeof(MeshData::Vertex);
    const uint64_t indexBytes = static_cast<uint64_t>(indexCount) * sizeof(MeshData::Index);

    DASSERT(_gStaticVertexBuffer.offset + vertexBytes <= _gStaticVertexBuffer.size());
    DASSERT(_gStaticIndexBuffer.offset + indexBytes <= _gStaticIndexBuffer.size());

    const uint64_t vertexElementOffset = _gStaticVertexBuffer.offset / sizeof(MeshData::Vertex);
    const uint64_t indexElementOffset = _gStaticIndexBuffer.offset / sizeof(MeshData::Index);

    result.vertexOffset = static_cast<uint>(vertexElementOffset);
    result.vertexCount = static_cast<uint>(vertexCount);
    result.indexOffset = static_cast<uint>(indexElementOffset);
    result.indexCount = static_cast<uint>(indexCount);
    result.center = mesh.center;
    result.radius = mesh.radius;

    _gStaticVertexBuffer.write(mesh.vertices.data(), vertexBytes);
    _gStaticIndexBuffer.write(mesh.indices.data(), indexBytes);

    return result;
}


}
