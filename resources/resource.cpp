#include "resource.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cstdint>

namespace resources {

bool loadObjFile(std::filesystem::path path, MeshData& md) noexcept
{
    md.path = std::move(path);
    md.vertices.clear();
    md.indices.clear();

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

    return !md.vertices.empty() && !md.indices.empty();
}

} // namespace resources
