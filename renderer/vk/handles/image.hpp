#pragma once

#include "simemory_accessor.hpp"

namespace vk { namespace handles {

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
END_DECLARE_VKSTRUCT()

class Image
    : public SIMemoryAccessor<Image>
    , public Handle<VkImage>
{
public:
    Image(Image&& other) noexcept;
    Image(const Image& other) = delete;
    Image(const Device& device, VkHandleType* handle);
    Image(const Device& device, ImageCreateInfo imageInfo, VkHandleType* handlePtr = nullptr);
    ~Image();

    bool bindMemory(uint32_t bindingOffset);
    std::weak_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties)
	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, handle(), &memRequirements);

		m_memory = std::make_shared<Memory>(m_device,
			memoryAllocateInfo(memRequirements.size,
				utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits,
					properties)));

		return m_memory;
	}


    void transitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
};

}}    //  namespace vk::handles
