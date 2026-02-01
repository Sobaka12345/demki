#ifndef RENDERER_VK_DEFAULT_RENDERER_HPP
#define RENDERER_VK_DEFAULT_RENDERER_HPP

#include "../renderer_fwd.hpp"

#include <array>
#include <gapi_context.hpp>
#include <swapchain.hpp>
#include <vulkan/vulkan_core.h>

namespace renderer::__private {

template <>
struct DefaultRenderer<Vk>
{
    struct Context : gapi::GApiContext<Vk>
    {
        std::vector<VkFence> inFlightFences;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderWaitSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;

        uint32_t currentFrameInFlight = 0;
        uint32_t maxFramesInFlight = 2;

        VkDevice device = VK_NULL_HANDLE;
        
        std::array<VkQueue, QueueFamily::Type::COUNT> queues;
        std::array<VkCommandPool, QueueFamily::Type::COUNT> commandPools;
        std::array<std::vector<VkCommandBuffer>, QueueFamily::Type::COUNT> commandBuffers;

        PhysicalDevice* physicalDeviceInUse = nullptr;
        std::vector<PhysicalDevice> suitablePhysicalDevices;
        gapi::Swapchain<Vk> swapchain;
    };

    static void createDevice(Context& ctx) noexcept;
    static void destroyDevice(Context& ctx) noexcept;

    static void createSynchronization(Context& ctx) noexcept;
    static void destroySynchronization(Context& ctx) noexcept;

    static void createSwapchain(Context& ctx) noexcept;
    static void destroySwapchain(Context& ctx) noexcept;
    static void recreateSwapchain(Context& ctx) noexcept;
    static void pickSuitablePhysicalDevices(Context& ctx) noexcept;

    // SIRenderer static interface
    static Context init(gapi::GApiContext<Vk>& ctx);
    static void teardown(Context& ctx);
    static void render(Context& ctx);
    static void prepareSwapchain(Context& ctx);
    static void presentSwapchain(Context& ctx);
};

}

#endif // RENDERER_VK_DEFAULT_RENDERER_HPP