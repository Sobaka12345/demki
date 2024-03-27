#include "resources.hpp"
#include <assert.hpp>

namespace shell {

Resources::Resources(std::filesystem::path root) noexcept
    : m_root(root)
{}

void Resources::registerResource(std::weak_ptr<IResource> resource) {}

}
