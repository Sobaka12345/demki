#pragma once

#include <filesystem>

class Resources
{
public:
    explicit Resources(std::filesystem::path root) noexcept;

    template <typename T>
    T* registerResource(T* resource)
    {
        return resource;
    }

private:
    std::filesystem::path m_root;
};
