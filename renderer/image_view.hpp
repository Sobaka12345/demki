#pragma once

#include "device.hpp"

namespace vk {

class ImageView : public Handle<VkImageView>
{
public:
    ImageView(const ImageView& other) = delete;
    ImageView(ImageView&& other) noexcept;
    ImageView(const Device& device, VkHandleType* handlePtr) noexcept;
    ImageView(const Device& device, VkImageViewCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~ImageView();

private:
    const Device& m_device;
};

}