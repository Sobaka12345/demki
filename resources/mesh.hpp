#pragma once

#include "iresource.hpp"

#include <filesystem>
#include <span>

namespace resources {

class OperationContext;

struct Mesh : public IResource
{
    explicit Mesh(std::filesystem::path path) noexcept;
    explicit Mesh(std::span<const Vertex3DColoredTextured> vertices,
        std::span<const uint32_t> indices) noexcept;
    virtual ~Mesh() {};

    std::vector<Vertex3DColoredTextured> vertices;
    std::vector<uint16_t> indices;

    size_t sizeBytes() const;
};

}    //  namespace resources
