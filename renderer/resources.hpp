#pragma once

#include "vertex.hpp"

#include <filesystem>

class Resources
{
public:
    explicit Resources(std::filesystem::path root) noexcept;
    std::pair<std::vector<Vertex3DColoredTextured>, std::vector<uint32_t>> loadModelData(
        std::filesystem::path path) noexcept;

    template <typename T>
    T* registerResource(T* resource)
    {
        return resource;
    }

private:
    std::filesystem::path m_root;
};
