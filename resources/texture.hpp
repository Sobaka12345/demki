#pragma once

#include "iresource.hpp"

#include <filesystem>

namespace resources {

struct Image : public IResource
{
public:
    explicit Image(std::filesystem::path path) noexcept;
    virtual ~Image() override;

    virtual size_t size() const override;

private:
    int32_t m_width;
    int32_t m_height;
    int32_t m_channels;
    void* m_pixels = nullptr;

};

}    //  namespace resources
