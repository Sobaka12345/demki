#pragma once

#include <itexture.hpp>

#include <memory>

namespace vk {

class GraphicsContext;

namespace handles {
class Image;
class ImageView;
class Sampler;
}

class Texture : public ITexture
{
public:
    Texture(const GraphicsContext& context, ITexture::CreateInfo createInfo);
    ~Texture();

    virtual void bind(::RenderContext& context) override;
    virtual std::shared_ptr<IUniformHandle> uniformHandle() override;

private:
    void generateMipmaps();

private:
    const GraphicsContext& m_context;

    uint32_t m_mipLevels;
    int m_width;
    int m_height;

    std::shared_ptr<IUniformHandle> m_handle;
    std::unique_ptr<handles::Image> m_image;
    std::unique_ptr<handles::ImageView> m_imageView;
    std::unique_ptr<handles::Sampler> m_sampler;
};

}    //  namespace vk
