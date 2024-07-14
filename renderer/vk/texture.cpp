#include "texture.hpp"

#include "graphics_context.hpp"
#include "types.hpp"
#include "shader_interface_handle.hpp"

#include "handles/buffer.hpp"
#include "handles/image.hpp"
#include "handles/image_view.hpp"
#include "handles/sampler.hpp"

namespace {
constexpr auto s_imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
}

namespace renderer::vk {

Texture::Texture(const GraphicsContext& context, ITexture::CreateInfo createInfo)
    : m_context(context)
    , m_width(createInfo.width)
    , m_height(createInfo.height)
{
    m_mipLevels = static_cast<uint32_t>(std::floor(std::log2((std::max)(m_width, m_height)))) + 1;

    ASSERT(createInfo.pixels, "failed to load texture image!");

    handles::Buffer stagingBuffer(m_context.device(),
        handles::Buffer::staging().size(createInfo.imageSize));
    stagingBuffer
        .allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        .lock()
        ->map()
        .lock()
        ->write(createInfo.pixels, createInfo.imageSize);

    m_image = std::make_unique<handles::Image>(m_context.device(),
        handles::ImageCreateInfo()
            .imageType(VK_IMAGE_TYPE_2D)
            .extent(
                VkExtent3D{ static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 })
            .mipLevels(m_mipLevels)
            .arrayLayers(1)
            .format(s_imageFormat)
            .tiling(VK_IMAGE_TILING_OPTIMAL)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .usage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                VK_IMAGE_USAGE_SAMPLED_BIT)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_image->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    const auto subresourceRange =
        ImageSubresourceRange{}
            .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .baseArrayLayer(0)
            .layerCount(1)
            .baseMipLevel(0)
            .levelCount(m_mipLevels);

    m_image->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresourceRange);

    const auto copyRegion =
        BufferImageCopy{}
            .bufferOffset(0)
            .bufferImageHeight(0)
            .bufferRowLength(0)
            .imageSubresource(
                ImageSubresourceLayers{}
                    .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
                    .baseArrayLayer(0)
                    .layerCount(1)
                    .mipLevel(0))
            .imageOffset(VkOffset3D{ 0, 0, 0 })
            .imageExtent(
                VkExtent3D{ static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 });

    stagingBuffer.copyToImage(*m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegion);

    generateMipmaps();

    m_imageView = std::make_unique<handles::ImageView>(m_context.device(),
        handles::ImageViewCreateInfo()
            .image(*m_image)
            .viewType(VK_IMAGE_VIEW_TYPE_2D)
            .format(s_imageFormat)
            .subresourceRange(subresourceRange));

    m_sampler = std::make_unique<handles::Sampler>(m_context.device(),
        handles::SamplerCreateInfo()
            .magFilter(VK_FILTER_LINEAR)
            .minFilter(VK_FILTER_LINEAR)
            .mipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
            .minLod(0.0f)
            .maxLod(m_mipLevels)
            .mipLodBias(0.0f)
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

std::shared_ptr<ShaderResource::Descriptor> Texture::fetchDescriptor()
{
    auto result = ShaderResource::fetchDescriptor();
    result->descriptorImageInfo.imageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        .imageView(*m_imageView)
        .sampler(*m_sampler);

    return result;
}

void Texture::freeDescriptor(const ShaderResource::Descriptor& descriptor) {}

std::shared_ptr<IShaderInterfaceHandle> Texture::uniformHandle()
{
    if (!m_handle)
    {
        auto handle = ShaderInterfaceHandle::create(*this);
        m_handle = handle;
    }

    return m_handle;
}

void Texture::generateMipmaps()
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_context.device().physicalDevice(), s_imageFormat,
        &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    auto oneTimeCommand = m_context.device().oneTimeCommand(handles::GRAPHICS_COMPUTE);

    auto barrier =
        ImageMemoryBarrier{}
            .image(*m_image)
            .srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .subresourceRange(
                ImageSubresourceRange{}
                    .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
                    .baseArrayLayer(0)
                    .layerCount(1)
                    .levelCount(1));

    int32_t mipWidth = m_width;
    int32_t mipHeight = m_height;

    for (uint32_t i = 1; i < m_mipLevels; i++)
    {
        barrier.oldLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            .newLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            .srcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT)
            .dstAccessMask(VK_ACCESS_TRANSFER_READ_BIT)
            .subresourceRange()
            .baseMipLevel(i - 1);

        oneTimeCommand().pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, std::span{ &barrier, 1 });

        auto blit = ImageBlit{};
        blit.srcOffsets()[0] = Offset3D{}.x(0).y(0).z(0);
        blit.srcOffsets()[1] = Offset3D{}.x(mipWidth).y(mipHeight).z(1);
        blit.dstOffsets()[0] = Offset3D{}.x(0).y(0).z(0);
        blit.dstOffsets()[1] =
            Offset3D{}.x(mipWidth > 1 ? mipWidth / 2 : 1).y(mipHeight > 1 ? mipHeight / 2 : 1).z(1);
        blit.srcSubresource(
                ImageSubresourceLayers{}
                    .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
                    .baseArrayLayer(0)
                    .layerCount(1)
                    .mipLevel(i - 1))
            .dstSubresource(
                ImageSubresourceLayers{}
                    .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
                    .baseArrayLayer(0)
                    .layerCount(1)
                    .mipLevel(i));

        oneTimeCommand().blitImage(*m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, std::span{ &blit, 1 }, VK_FILTER_LINEAR);

        barrier.oldLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            .newLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            .srcAccessMask(VK_ACCESS_TRANSFER_READ_BIT)
            .dstAccessMask(VK_ACCESS_SHADER_READ_BIT);

        oneTimeCommand().pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, std::span{ &barrier, 1 });

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.srcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT)
        .dstAccessMask(VK_ACCESS_SHADER_READ_BIT)
        .oldLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        .newLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        .subresourceRange()
        .baseMipLevel(m_mipLevels - 1);

    oneTimeCommand().pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, std::span{ &barrier, 1 });
}

}    //  namespace renderer::vk
