#pragma once

#include "iresource.hpp"

#include <filesystem>
#include <span>

namespace resources {

class OperationContext;

class Mesh : public IResource
{
public:
    explicit Mesh(std::filesystem::path path) noexcept;
    explicit Mesh(std::span<const Vertex3DColoredTextured> vertices,
        std::span<const uint32_t> indices) noexcept;

public:
    virtual ~Mesh() {};
};

}    //  namespace resources
