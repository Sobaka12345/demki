#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(BufferCreateInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkBufferCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkDeviceSize, size)
    VKSTRUCT_PROPERTY(VkBufferUsageFlags, usage)
    VKSTRUCT_PROPERTY(VkSharingMode, sharingMode)
    VKSTRUCT_PROPERTY(uint32_t, queueFamilyIndexCount)
    VKSTRUCT_PROPERTY(const uint32_t*, pQueueFamilyIndices)
END_DECLARE_VKSTRUCT()

template <typename T>
struct BufferFunctions : public CRTPBase<T>
{
    CREATE_FUNC(Buffer, Device);
    DESTROY_FUNC(Buffer, Device);

    static constexpr inline BufferCreateInfo stagingInfo() noexcept
    {
        return BufferCreateInfo()
            .usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE);
    }
};

template <typename T>
struct BufferGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Buffer, BufferFunctions, BufferGroupFunctions);
}

}    //  namespace renderer::vk
