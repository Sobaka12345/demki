#include "image.hpp"

namespace vk {

Image::Image(Image&& other)
    : HandleBase(std::move(other))
    , SIMemoryAccessor(std::move(other))
{}

Image::Image(const Device& device, VkImageCreateInfo imageInfo)
    : SIMemoryAccessor(device, imageInfo.sharingMode)
{
    ASSERT(vkCreateImage(device, &imageInfo, nullptr, &m_handle) == VK_SUCCESS);
}

Image::~Image()
{
    vkDestroyImage(m_device, handle(), nullptr);
}

bool Image::bindMemory(uint32_t bindingOffset)
{
    return vkBindImageMemory(m_device, handle(), *m_memory, bindingOffset) == VK_SUCCESS;
}

std::shared_ptr<Memory> Image::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, handle(), &memRequirements);

    m_memory = std::make_shared<Memory>(m_device, create::memoryAllocateInfo(
        memRequirements.size,
        utils::findMemoryType(m_device.physicalDevice(), memRequirements.memoryTypeBits, properties))
    );

    return m_memory;
}

}
