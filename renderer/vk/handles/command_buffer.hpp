#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

#include <functional>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(CommandBufferAllocateInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO)
    VKSTRUCT_PROPERTY(VkStructureType, sType)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandPool, commandPool)
    VKSTRUCT_PROPERTY(VkCommandBufferLevel, level)
    VKSTRUCT_PROPERTY(uint32_t, commandBufferCount)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(CommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkCommandBufferUsageFlags, flags)
    VKSTRUCT_PROPERTY(const VkCommandBufferInheritanceInfo*, pInheritanceInfo)
END_DECLARE_VKSTRUCT()

template <typename T>
struct CommandBufferFunctions : public CRTPBase<T>
{
    struct OneTimeCommand
    {
        OneTimeCommand(VkDevice device, VkCommandPool pool, VkCommandBufferLevel level) noexcept
            : device(device)
            , pool(pool)
            , level(level)
        {
            const auto allocateInfo =
                CommandBufferAllocateInfo{}.commandPool(pool).level(level).commandBufferCount(1);
            handle = T::create(device, &allocateInfo);
            const auto beginInfo =
                CommandBufferBeginInfo{}
                    .flags(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
                    .pInheritanceInfo(nullptr);

            ASSERT(vkBeginCommandBuffer(handle, &beginInfo) == VK_SUCCESS);
        }

        operator typename T::Handle() const { return handle; }

        inline static bool exec(VkDevice device,
            VkCommandPool pool,
            VkCommandBufferLevel level,
            std::function<void(typename T::Handle)> writeCommands) noexcept
        {
            OneTimeCommand command{ device, pool, level };
            writeCommands(command);
        }

        ~OneTimeCommand() noexcept
        {
            ASSERT(vkEndCommandBuffer(handle) == VK_SUCCESS);
            T::destroy(device, pool, 1, &handle);
        }

        T::Handle handle;
        VkDevice device;
        VkCommandPool pool;
        VkCommandBufferLevel level;
    };
};

template <typename T>
struct CommandBufferGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE_FULL(CommandBuffer,
    vkAllocateCommandBuffers,
    vkFreeCommandBuffers,
    CommandBufferFunctions,
    CommandBufferGroupFunctions);
}

}    //  namespace renderer::vk
