#pragma once

#include <iresources.hpp>

#include <filesystem>

namespace shell {

class Resources : public IResources
{
public:
    explicit Resources(std::filesystem::path root) noexcept;
    virtual void registerResource(std::weak_ptr<IResource> resource) override;

private:
    std::filesystem::path m_root;
};

}    //  namespace shell
