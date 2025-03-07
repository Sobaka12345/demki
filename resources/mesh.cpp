#include "mesh.hpp"

namespace resources {

Mesh::Mesh(std::filesystem::path path) noexcept {}

Mesh::Mesh(std::span<const Vertex3DColoredTextured> vertices,
    std::span<const uint32_t> indices) noexcept
{}


}    //  namespace resources
