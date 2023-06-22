#pragma once

#include "memory.hpp"

namespace vk {

class Image : public SIMemoryAccessor<Image>, public Handle<VkImage>
{
public:
    Image(Image&& other) noexcept;
    Image(const Image& other) = delete;
    Image(const Device& device, VkHandleType* handle);
    Image(const Device& device, VkImageCreateInfo imageInfo, VkHandleType* handlePtr = nullptr);
    ~Image();

    bool bindMemory(uint32_t bindingOffset);
    std::shared_ptr<Memory> allocateMemory(VkMemoryPropertyFlags properties);
};

}
