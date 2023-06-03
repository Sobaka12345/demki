#include "image.hpp"

namespace vk {
    Image::Image(Image&& other)
    : SIMemoryAccessor(std::move(other))
    , m_image(std::move(other.m_image))
{
    other.m_image = VK_NULL_HANDLE;
}

Image::Image(const Device& device, VkImageCreateInfo imageInfo)
    : SIMemoryAccessor(device, imageInfo.sharingMode)
{
    ASSERT(vkCreateImage(device, &imageInfo, nullptr, &m_image) == VK_SUCCESS);
}

Image::~Image()
{
    vkDestroyImage(m_device, m_image, nullptr);
}

bool Image::bindMemory(uint32_t bindingOffset)
{
    return vkBindImageMemory(m_device, m_image, m_memory->deviceMemory, bindingOffset) == VK_SUCCESS;
}

std::shared_ptr<Memory> Image::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

    m_memory = std::make_shared<Memory>(m_device, create::memoryAllocateInfo(
        memRequirements.size,
        utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits, properties))
    );

    return m_memory;
}

}
