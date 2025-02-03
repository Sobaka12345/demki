#include "texture.hpp"

#include "graphics_context.hpp"

#include "types.hpp"
#include "handles/buffer.hpp"
#include "handles/memory.hpp"
#include "handles/image.hpp"
#include "handles/image_view.hpp"
#include "handles/sampler.hpp"

namespace {
constexpr auto s_imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
}

namespace renderer::vk {

using namespace handles;

Texture::Texture(GraphicsContext& context, ITexture::CreateInfo createInfo)
    : m_context(context)
    , m_width(createInfo.width)
    , m_height(createInfo.height)
{
    m_mipLevels = createInfo.mipLevels;

    ASSERT(createInfo.pixels, "failed to load texture image!");

    auto stagingBufferCreateInfo = handles::BufferHelper::stagingInfo().size(createInfo.imageSize);
    auto stagingBuffer =
        handles::BufferHelper::create(m_context.device(), &stagingBufferCreateInfo, nullptr);
    auto stagingMemoryAllocateInfo =
        MemoryAllocateInfo{}
            .allocationSize(createInfo.imageSize)
            .memoryTypeIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mem;
    auto memory = handles::DeviceMemoryHelper::create(m_context.device(),
        &stagingMemoryAllocateInfo, nullptr);
    vkMapMemory(m_context.device(), memory, 0, createInfo.imageSize, 0, &mem);
    vkBindBufferMemory(m_context.device(), stagingBuffer, memory, 0);

    //		->write(createInfo.pixels, createInfo.imageSize);

    const auto imageCreateInfo =
        ImageCreateInfo()
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
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE);

    m_image = ImageHelper::create(m_context.device(), &imageCreateInfo, nullptr);

    //  m_image->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    const auto subresourceRange =
        ImageSubresourceRange{}
            .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .baseArrayLayer(0)
            .layerCount(1)
            .baseMipLevel(0)
            .levelCount(m_mipLevels);

    handles::ImageHelper::transitionLayout(m_context.device(),
        m_context.commandPool(QueueFamilyType::GRAPHICS_COMPUTE), m_image,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

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

    //  stagingBuffer.copyToImage(*m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegion);

    generateMipmaps();

    auto imageViewCreateInfo =
        ImageViewCreateInfo()
            .image(m_image)
            .viewType(VK_IMAGE_VIEW_TYPE_2D)
            .format(s_imageFormat)
            .subresourceRange(subresourceRange);
    m_imageView = ImageViewHelper::create(m_context.device(), &imageViewCreateInfo, nullptr);

    auto samplerCreateInfo =
        SamplerCreateInfo()
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
            .maxAnisotropy(m_context.physicalDeviceInfo().properties.limits.maxSamplerAnisotropy)
            .compareEnable(VK_FALSE)
            .compareOp(VK_COMPARE_OP_ALWAYS)
            .borderColor(VK_BORDER_COLOR_INT_OPAQUE_BLACK)
            .unnormalizedCoordinates(VK_FALSE);

    m_sampler = SamplerHelper::create(m_context.device(), &samplerCreateInfo, nullptr);
}

Texture::~Texture()
{
    SamplerHelper::destroy(m_context.device(), m_sampler, nullptr);
    ImageViewHelper::destroy(m_context.device(), m_imageView, nullptr);
    ImageHelper::destroy(m_context.device(), m_image, nullptr);
}

void Texture::bind(renderer::OperationContext& context, uint32_t bindingId) const
{
    //  NOTHING TO DO
}

void Texture::generateMipmaps()
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_context.physicalDevice(), s_imageFormat,
        &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    auto oneTimeCommand = CommandBufferHelper::OneTimeCommand{ m_context.device(),
        m_context.commandPool(QueueFamilyType::GRAPHICS_COMPUTE), VK_COMMAND_BUFFER_LEVEL_PRIMARY };

    auto barrier =
        ImageMemoryBarrier{}
            .image(m_image)
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

        vkCmdPipelineBarrier(oneTimeCommand, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

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

        vkCmdBlitImage(oneTimeCommand, m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            .newLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            .srcAccessMask(VK_ACCESS_TRANSFER_READ_BIT)
            .dstAccessMask(VK_ACCESS_SHADER_READ_BIT);

        vkCmdPipelineBarrier(oneTimeCommand, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.srcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT)
        .dstAccessMask(VK_ACCESS_SHADER_READ_BIT)
        .oldLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        .newLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        .subresourceRange()
        .baseMipLevel(m_mipLevels - 1);

    vkCmdPipelineBarrier(oneTimeCommand, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

}    //  namespace renderer::vk
