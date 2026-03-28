#include "resources.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace resources {

bool loadObjFile(MeshData& md) noexcept
{
    md.vertices.clear();
    md.indices.clear();
    md.center = vec3 { 0.0f, 0.0f, 0.0f };
    md.radius = 0.0f;

    Assimp::Importer importer {};
    const aiScene* scene = importer.ReadFile(md.path.string(),
        aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_FlipUVs);

    if (!scene || !scene->HasMeshes())
    {
        return false;
    }

    std::size_t totalVertices = 0;
    std::size_t totalIndices = 0;
    for (unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (!mesh || !mesh->HasPositions()) continue;

        totalVertices += mesh->mNumVertices;
        for (unsigned f = 0; f < mesh->mNumFaces; ++f)
        {
            totalIndices += scene->mMeshes[i]->mFaces[f].mNumIndices;
        }
    }

    md.vertices.reserve(totalVertices);
    md.indices.reserve(totalIndices);

    uint32_t vertexBase = 0;
    for (unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (!mesh || !mesh->HasPositions()) continue;

        for (unsigned v = 0; v < mesh->mNumVertices; ++v)
        {
            MeshData::Vertex out {};
            out.color = vec3 { 1.0f, 1.0f, 1.0f };

            const aiVector3D& pos = mesh->mVertices[v];
            out.pos = vec3 { pos.x, pos.y, pos.z };

            if (mesh->HasTextureCoords(0))
            {
                const aiVector3D& uv = mesh->mTextureCoords[0][v];
                out.texture = vec2 { uv.x, uv.y };
            }

            md.vertices.push_back(out);
        }

        for (unsigned f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned j = 0; j < face.mNumIndices; ++j)
            {
                const auto index = static_cast<uint32_t>(face.mIndices[j]) + vertexBase;
                md.indices.push_back(static_cast<MeshData::Index>(index));
            }
        }

        vertexBase += mesh->mNumVertices;
    }

    if (md.vertices.empty() || md.indices.empty()) return false;

    vec3 minPos = md.vertices.front().pos;
    vec3 maxPos = md.vertices.front().pos;
    for (const MeshData::Vertex& vertex : md.vertices)
    {
        const vec3& pos = vertex.pos;
        minPos.x = std::min(minPos.x, pos.x);
        minPos.y = std::min(minPos.y, pos.y);
        minPos.z = std::min(minPos.z, pos.z);
        maxPos.x = std::max(maxPos.x, pos.x);
        maxPos.y = std::max(maxPos.y, pos.y);
        maxPos.z = std::max(maxPos.z, pos.z);
    }

    md.center = (minPos + maxPos) * 0.5f;
    float maxDistanceSquared = 0.0f;
    for (const MeshData::Vertex& vertex : md.vertices)
    {
        const vec3 delta = vertex.pos - md.center;
        const float distanceSquared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        maxDistanceSquared = std::max(maxDistanceSquared, distanceSquared);
    }
    md.radius = std::sqrt(maxDistanceSquared);

    return true;
}


ResourcesBase::MeshHandle ResourcesBase::addMesh(std::filesystem::path filePath) noexcept
{
    auto result = _meshes.insert(_meshes.cend(), MeshData{ 
        .path = std::move(filePath) 
    });
    loadObjFile(*result);
    return result;
}

} // namespace resources
