#pragma once

#include "iresources.hpp"

#include <filesystem>

namespace resources {

class Resources : public IResources
{
public:
    explicit Resources(std::filesystem::path root) noexcept;

public:


private:
    std::filesystem::path m_root;
};

}    //  namespace shell
