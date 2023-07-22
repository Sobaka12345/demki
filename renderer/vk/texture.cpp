#include "texture.hpp"

#include "graphics_context.hpp"

#include "handles/buffer.hpp"
#include "handles/image.hpp"
#include "handles/image_view.hpp"
#include "handles/sampler.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vk {

Texture::Texture(const GraphicsContext &context, ITexture::CreateInfo createInfo)
    : m_context(context)
{
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels =
        stbi_load(createInfo.path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    ASSERT(pixels, "failed to load texture image!");

    handles::Buffer stagingBuffer(m_context.device(), handles::Buffer::staging().size(imageSize));
    stagingBuffer
        .allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        .lock()
        ->map()
        .lock()
        ->write(pixels, imageSize);

    m_image = std::make_unique<handles::Image>(m_context.device(),
        handles::ImageCreateInfo()
            .imageType(VK_IMAGE_TYPE_2D)
            .extent({ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 })
            .mipLevels(1)
            .arrayLayers(1)
            .format(VK_FORMAT_R8G8B8A8_SRGB)
            .tiling(VK_IMAGE_TILING_OPTIMAL)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_image->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_image->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const auto copyRegion = handles::bufferImageCopy(0,
        0,
        0,
        handles::imageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1),
        { 0, 0, 0 },
        { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 });

    stagingBuffer.copyToImage(*m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegion);

    m_image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stbi_image_free(pixels);

    m_imageView = std::make_unique<handles::ImageView>(m_context.device(),
        handles::ImageViewCreateInfo()
            .image(*m_image)
            .viewType(VK_IMAGE_VIEW_TYPE_2D)
            .format(VK_FORMAT_R8G8B8A8_SRGB)
            .subresourceRange(
                handles::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)));

    m_sampler = std::make_unique<handles::Sampler>(m_context.device(),
        handles::SamplerCreateInfo()
            .magFilter(VK_FILTER_LINEAR)
            .minFilter(VK_FILTER_LINEAR)
            .mipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
            .addressModeU(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .addressModeV(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .addressModeW(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .anisotropyEnable(VK_FALSE)
            .maxAnisotropy(
                m_context.device().physicalDeviceProperties().limits.maxSamplerAnisotropy)
            .compareEnable(VK_FALSE)
            .compareOp(VK_COMPARE_OP_ALWAYS)
            .borderColor(VK_BORDER_COLOR_INT_OPAQUE_BLACK)
            .unnormalizedCoordinates(VK_FALSE));
}

Texture::~Texture()
{
    m_sampler.reset();
    m_imageView.reset();
    m_image.reset();
}

void Texture::bind(::RenderContext &context) {}

std::shared_ptr<IUniformHandle> Texture::uniformHandle()
{
    if (!m_handle)
    {
        auto handle = UniformHandle::create();
        handle->descriptorImageInfo.imageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            .imageView(*m_imageView)
            .sampler(*m_sampler);
        handle->resourceId = 555;
        m_handle = handle;
    }

    return m_handle;
}

}    //  namespace vk
