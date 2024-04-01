#include "image.hpp"

#include "device.hpp"
#include "memory.hpp"
#include "swapchain.hpp"

namespace renderer::vk { namespace handles {

HandleVector<Image> Image::swapChainImages(const Device& device, const Swapchain& swapchain)
{
    HandleVector<Image> result;
    uint32_t imageCount;

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);

    result.emplaceBackBatch(vkGetSwapchainImagesKHR, imageCount,
        std::forward_as_tuple(device.handle(), swapchain.handle(), &imageCount),
        std::forward_as_tuple(device));

    return result;
}

Image::Image(Image&& other) noexcept
    : Handle(std::move(other))
    , SIMemoryAccessor(std::move(other))
{}

Image::Image(const Device& device, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , SIMemoryAccessor(device)
{}

Image::Image(const Device& device, ImageCreateInfo imageInfo, VkHandleType* handlePtr) noexcept
    : Image(device, handlePtr)
{
    ASSERT(create(vkCreateImage, device, &imageInfo, nullptr) == VK_SUCCESS,
        "failed to create image!");
}

Image::Image(const Device& device, ImageCreateInfo imageInfo) noexcept
    : Image(device, std::move(imageInfo), nullptr)
{}

Image::~Image()
{
    destroy(vkDestroyImage, m_device, handle(), nullptr);
}

bool Image::bindMemory(uint32_t bindingOffset)
{
    DASSERT(m_memory);
    return m_memory->bindImage(*this, bindingOffset);
}

void Image::transitionLayout(
    VkImageLayout oldLayout, VkImageLayout newLayout, ImageSubresourceRange subresourceRange)
{
    auto oneTimeCommand = m_device.oneTimeCommand(GRAPHICS_COMPUTE);

    auto barrier =
        ImageMemoryBarrier{}
            .oldLayout(oldLayout)
            .newLayout(newLayout)
            .srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .image(handle())
            .subresourceRange(subresourceRange);

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask(0);
        barrier.dstAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
        barrier.dstAccessMask(VK_ACCESS_SHADER_READ_BIT);

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    oneTimeCommand().pipelineBarrier(sourceStage, destinationStage, 0,
        std::span<ImageMemoryBarrier, 1>(&barrier, 1));
}

std::weak_ptr<Memory> Image::allocateMemoryImpl(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, handle(), &memRequirements);

    m_memory = std::make_shared<Memory>(m_device,
        MemoryAllocateInfo{}
            .allocationSize(memRequirements.size)
            .memoryTypeIndex(findMemoryType(m_device.physicalDevice(),
                memRequirements.memoryTypeBits, properties)));

    return m_memory;
}

}}    //  namespace renderer::vk::handles
