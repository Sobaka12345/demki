#pragma once

#include "handles/image_view.hpp"
#include "handles/sampler.hpp"

#include <itexture.hpp>

namespace renderer::vk {

class GraphicsContext;

class Texture : public ITexture
{
public:
    Texture(GraphicsContext& context, ITexture::CreateInfo createInfo);
    ~Texture();

    //  IShaderResource interface
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;

    void generateMipmaps();

private:
    const GraphicsContext& m_context;

    uint32_t m_mipLevels;
    int m_width;
    int m_height;

    VkImage m_image;
    VkImageView m_imageView;
    VkSampler m_sampler;
};

}    //  namespace renderer::vk
