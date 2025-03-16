#pragma once

#include "iresource.hpp"

#include <filesystem>

namespace resources {

struct Image : public IResource
{
public:
    explicit Image(std::filesystem::path path) noexcept;

    virtual ~Image() {}


};

}    //  namespace resources
