#pragma once

#include "handle.hpp"
#include "command_buffer.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageSubresourceLayers)
    VKSTRUCT_PROPERTY(VkImageAspectFlags, aspectMask)
    VKSTRUCT_PROPERTY(uint32_t, mipLevel)
    VKSTRUCT_PROPERTY(uint32_t, baseArrayLayer)
    VKSTRUCT_PROPERTY(uint32_t, layerCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageSubresourceRange)
    VKSTRUCT_PROPERTY(VkImageAspectFlags, aspectMask)
    VKSTRUCT_PROPERTY(uint32_t, baseMipLevel)
    VKSTRUCT_PROPERTY(uint32_t, levelCount)
    VKSTRUCT_PROPERTY(uint32_t, baseArrayLayer)
    VKSTRUCT_PROPERTY(uint32_t, layerCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(ImageMemoryBarrier, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkAccessFlags, srcAccessMask)
    VKSTRUCT_PROPERTY(VkAccessFlags, dstAccessMask)
    VKSTRUCT_PROPERTY(VkImageLayout, oldLayout)
    VKSTRUCT_PROPERTY(VkImageLayout, newLayout)
    VKSTRUCT_PROPERTY(uint32_t, srcQueueFamilyIndex)
    VKSTRUCT_PROPERTY(uint32_t, dstQueueFamilyIndex)
    VKSTRUCT_PROPERTY(VkImage, image)
    VKSTRUCT_PROPERTY(ImageSubresourceRange, subresourceRange)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_UNTYPED_VKSTRUCT(ImageBlit)
    VKSTRUCT_PROPERTY(ImageSubresourceLayers, srcSubresource)
    VKSTRUCT_PROPERTY(std::span<const VkOffset3D COMMA 2>, srcOffsets)
    VKSTRUCT_PROPERTY(ImageSubresourceLayers, dstSubresource)
    VKSTRUCT_PROPERTY(std::span<const VkOffset3D COMMA 2>, dstOffsets)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(ImageCreateInfo, VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkImageCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkImageType, imageType)
    VKSTRUCT_PROPERTY(VkFormat, format)
    VKSTRUCT_PROPERTY(VkExtent3D, extent)
    VKSTRUCT_PROPERTY(uint32_t, mipLevels)
    VKSTRUCT_PROPERTY(uint32_t, arrayLayers)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, samples)
    VKSTRUCT_PROPERTY(VkImageTiling, tiling)
    VKSTRUCT_PROPERTY(VkImageUsageFlags, usage)
    VKSTRUCT_PROPERTY(VkSharingMode, sharingMode)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndexCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pQueueFamilyIndices)
    VKSTRUCT_PROPERTY(VkImageLayout, initialLayout)
END_DECLARE_VKSTRUCT();

template <typename T>
struct ImageFunctions : public CRTPBase<T>
{
    CREATE_FUNC(Image, Device);
    DESTROY_FUNC(Image, Device);

    static inline auto swapChainImages(VkDevice device, VkSwapchainKHR swapchain) noexcept
    {
        typename T::template Vector<> result;
        uint32_t imageCount;

        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        result.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, result.data());

        return result;
    }

    //  Rework
    static inline void transitionLayout(VkDevice device,
        VkCommandPool pool,
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        ImageSubresourceRange subresourceRange) noexcept
    {
        auto barrier =
            ImageMemoryBarrier{}
                .oldLayout(oldLayout)
                .newLayout(newLayout)
                .srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                .dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                .image(image)
                .subresourceRange(subresourceRange);

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
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

        handles::CommandBuffer::OneTimeCommand::exec(device, pool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY, [&](auto buffer) {
                vkCmdPipelineBarrier(buffer, sourceStage, destinationStage, 0, 0, nullptr, 0,
                    nullptr, 1, &barrier);
            });

        //  const auto submitInfo =
        //      SubmitInfo{}.pCommandBuffers(&oneTimeCommand.handle).commandBufferCount(1);

        //  m_queue.submit(1, &submitInfo, VK_NULL_HANDLE);
        //  m_queue.waitIdle();
        //  vkQueueWaitIdle(queue)
    }
};

template <typename T>
struct ImageGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Image, ImageFunctions, ImageGroupFunctions);
}

}    //  namespace renderer::vk
