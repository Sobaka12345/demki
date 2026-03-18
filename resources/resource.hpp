#pragma once

#include <buffer.hpp>


// TO DO: generate header based on header with basic types in gapi project.
#ifndef GLSL_HOST
#define GLSL_HOST
#define RESOURCES_LOCAL_GLSL_HOST
#endif
#include "../renderer/shaders/shared_types.hpp"
#ifdef RESOURCES_LOCAL_GLSL_HOST
#undef RESOURCES_LOCAL_GLSL_HOST
#undef GLSL_HOST
#endif

#include <gapi_context.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <list>
#include <vector>
#include <filesystem>
#include <vulkan/vulkan.h>

namespace resources {

struct MeshData {
    using Vertex = Vertex3DColoredTextured;
    using Index = IndexType;

    std::filesystem::path path;
    std::vector<Vertex> vertices;
    std::vector<Index> indices;
};

bool loadObjFile(std::filesystem::path path, MeshData& md) noexcept;

template<typename GApiT>
struct Resource
{
    using MeshHandle = std::list<MeshData>::const_iterator;
    
    explicit Resource(gapi::GApiContext<GApiT>& ctx, std::filesystem::path rootPath);
    
    [[nodiscard]] MeshHandle addMesh(std::filesystem::path filePath) noexcept;
    [[nodiscard]] MeshView mapBundleToGPU(MeshHandle handle) noexcept;
    
private:
    Resource(std::filesystem::path rootPath)
        : _rootPath{std::move(rootPath)}
    {}

private:
    std::filesystem::path _rootPath;
    std::list<MeshData> _meshes;
};

} // namespace resources
