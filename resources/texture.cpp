#include "texture.hpp"

namespace resources {

Image::Image(std::filesystem::path path) noexcept
{
    pixels = stbi_load(path.string().c_str(), &width, &height, &textureChannels, STBI_rgb_alpha);
    imageSize = width * height * 4;
}

Image::~Image()
{
    if (pixels)
    {
        stbi_image_free(pixels);
    }
}


}    //  namespace resources
