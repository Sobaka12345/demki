#include "resources.hpp"
#include <assert.hpp>

#include <type_list.hpp>

namespace resources {

Resources::Resources(std::filesystem::path root) noexcept
    : m_root(root)
{}

}
