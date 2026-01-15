#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace resources {

Image::Image(std::filesystem::path path) noexcept
    : IResource(TYPE_IMAGE)
{   
    m_pixels = stbi_load(path.string().c_str(), &m_width, &m_height, &m_channels, STBI_rgb_alpha);
    std::cout << m_channels;
}

Image::~Image()
{
    if (m_pixels)
    {
        stbi_image_free(m_pixels);
    }
}


}    //  namespace resources
