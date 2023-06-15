#pragma once

#include "memory.hpp"

namespace vk {

struct ImageBuilder
{
    VkImageType              imageType = VK_IMAGE_TYPE_2D;
    VkFormat                 format ;
    VkExtent3D               extent;
    uint32_t                 mipLevels;
    uint32_t                 arrayLayers;
    VkSampleCountFlagBits    samples;
    VkImageTiling            tiling;
    VkImageUsageFlags        usage;
    VkSharingMode            sharingMode;
    uint32_t                 queueFamilyIndexCount;
    const uint32_t*          pQueueFamilyIndices;
    VkImageLayout            initialLayout;
};

class Image : public SIMemoryAccessor<Image>, public HandleBase<VkImage>
{
public:
    Image(Image&& other);
    Image(const Device& device, VkImageCreateInfo imageInfo);
    ~Image();

    bool bindMemory(uint32_t bindingOffset);
    std::shared_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties);

    void copyTo(const Image& Image, VkCommandPool commandPool, VkQueue queue, VkImageCopy copyRegion);
};

}
