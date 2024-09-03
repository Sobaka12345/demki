#pragma once

#include <itexture.hpp>

#include "shader_resource_allocator.hpp"
#include "specific_shader_resource.hpp"

#include <memory>

namespace renderer::vk {

class GraphicsContext;

namespace handles {
class Image;
class ImageView;
class Sampler;
}

class Texture
    : public SpecificShaderResource<ITexture>
    , public ShaderResourceAllocator
{
public:
    Texture(GraphicsContext& context, ITexture::CreateInfo createInfo);
    ~Texture();

    virtual std::shared_ptr<ShaderResourceAllocator::Descriptor> fetchDescriptor() override;

    //  IShaderResource interface
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const override;
    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) override;

private:
    virtual void freeDescriptor(const ShaderResourceAllocator::Descriptor& descriptor) override;

    void generateMipmaps();

private:
    const GraphicsContext& m_context;

    uint32_t m_mipLevels;
    int m_width;
    int m_height;

    std::shared_ptr<ShaderInterfaceHandle> m_handle;

    std::unique_ptr<handles::Image> m_image;
    std::unique_ptr<handles::ImageView> m_imageView;
    std::unique_ptr<handles::Sampler> m_sampler;
};

}    //  namespace renderer::vk
