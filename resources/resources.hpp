#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "plain_view.hpp"

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
    vec3 center = vec3 { 0.0f, 0.0f, 0.0f };
    float radius = 0.0f;
};

bool loadObjFile(MeshData& md) noexcept;

struct ResourcesBase
{
    ResourcesBase(std::filesystem::path rootPath)
        : _rootPath{std::move(rootPath)}
    {}
    
    using MeshHandle = std::list<MeshData>::const_iterator;
    [[nodiscard]] MeshHandle addMesh(std::filesystem::path filePath) noexcept;

protected:
    std::filesystem::path _rootPath;
    std::list<MeshData> _meshes;
};

template<typename GApiT>
struct Resources : ResourcesBase
{
    explicit Resources(gapi::GApiContext<GApiT>& ctx, std::filesystem::path rootPath) noexcept;

    [[nodiscard]] MeshView mapToGPU(MeshHandle handle) noexcept;
   
private:
    resources::PlainView<gapi::Buffer<GApiT>> _gStaticIndexBuffer;
    resources::PlainView<gapi::Buffer<GApiT>> _gStaticVertexBuffer;
};

} // namespace resources


#endif // RESOURCES_HPP
