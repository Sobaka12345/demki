#pragma once

#include "iresource.hpp"

#include <filesystem>

namespace resources {

class Texture : public IResource
{
public:
    explicit Texture(std::filesystem::path path) noexcept;
    explicit Texture(std::filesystem::path path, uint32_t mipLevels) noexcept;

public:
    virtual ~Texture() {}
};

}    //  namespace resources
