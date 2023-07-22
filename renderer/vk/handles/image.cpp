#include "image.hpp"

namespace vk { namespace handles {

Image::Image(Image&& other) noexcept
    : Handle(std::move(other))
    , SIMemoryAccessor(std::move(other))
{}

Image::Image(const Device& device, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
{}

Image::Image(const Device& device, ImageCreateInfo imageInfo, VkHandleType* handlePtr)
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
{
    ASSERT(create(vkCreateImage, device, &imageInfo, nullptr) == VK_SUCCESS,
        "failed to create image!");
}

Image::~Image()
{
    destroy(vkDestroyImage, m_device, handle(), nullptr);
}

bool Image::bindMemory(uint32_t bindingOffset)
{
    return vkBindImageMemory(m_device, handle(), *m_memory, bindingOffset) == VK_SUCCESS;
}


void Image::transitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
{
    auto oneTimeCommand = m_device.oneTimeCommand(GRAPHICS);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = handle();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    oneTimeCommand().pipelineBarrier(sourceStage, destinationStage, 0,
        std::span<VkImageMemoryBarrier, 1>(&barrier, 1));
}

}}    //  namespace vk::handles
