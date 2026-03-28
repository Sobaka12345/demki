#ifndef RENDERER_VK_DEFAULT_RENDERER_HPP
#define RENDERER_VK_DEFAULT_RENDERER_HPP

#include "../renderer_fwd.hpp"


#include <buffer.hpp>
#include <plain_view.hpp>

#include <array>
#include <gapi_context.hpp>
#include <swapchain.hpp>
#include <vulkan/vulkan_core.h>

namespace renderer:: __private {

template <RendererType typeId>
struct Renderer<Vk, std::integral_constant<RendererType, typeId>>;

template<> 
struct Renderer<Vk, std::integral_constant<RendererType, RendererType::DEFAULT>> 
{
    struct Context : gapi::GApiContext<Vk>
    {
        std::vector<VkFence> inFlightFences;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderWaitSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;

        struct Attachments {
            std::vector<VkAttachmentDescription> data;
            std::vector<VkAttachmentReference> references;
            uint32_t colorAttachmentCount = 0;

            [[nodiscard]] inline VkAttachmentDescription* colorAttachments() noexcept { return &data[0]; } 
            [[nodiscard]] inline VkAttachmentReference* colorAttachmentReferences() noexcept  { return &references[0]; } 
            [[nodiscard]] inline VkAttachmentDescription* resolveAttachments() noexcept { return &data[colorAttachmentCount]; }
            [[nodiscard]] inline VkAttachmentReference* resolveAttachmentReferences() noexcept { return &references[colorAttachmentCount]; }
            [[nodiscard]] inline VkAttachmentDescription* depthStencilAttachment() noexcept { return &data[colorAttachmentCount * 2]; }
            [[nodiscard]] inline VkAttachmentReference* depthStencilAttachmentReference() noexcept { return &references[colorAttachmentCount * 2]; }
        } attachments;

        struct FramebufferData {
            VkFramebuffer handle;
            std::vector<VkImageView> imageViewAttachments;
            std::vector<VkImage> images;
            std::vector<VkDeviceMemory> imageMemory;
        };
        std::vector<FramebufferData> framebuffers;

        uint32_t currentFrameInFlight = 0;
        uint32_t maxFramesInFlight = 2;

        std::array<VkQueue, QueueFamily::Type::COUNT> queues;
        std::array<VkCommandPool, QueueFamily::Type::COUNT> commandPools;
        std::array<std::vector<VkCommandBuffer>, QueueFamily::Type::COUNT> commandBuffers;
        
        resources::PlainView<gapi::Buffer<Vk>> staticVertexBuffer;
        resources::PlainView<gapi::Buffer<Vk>> staticIndexBuffer;

        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_8_BIT;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VkDescriptorSet> descriptorSets;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;
        std::vector<PhysicalDevice> suitablePhysicalDevices;
        gapi::Swapchain<Vk> swapchain;
    };

    static void pickSuitablePhysicalDevices(Context& ctx) noexcept;
    
    static void createDevice(Context& ctx) noexcept;
    static void destroyDevice(Context& ctx) noexcept;

    static void createSynchronization(Context& ctx) noexcept;
    static void destroySynchronization(Context& ctx) noexcept;

    static void createSwapchain(Context& ctx) noexcept;
    static void destroySwapchain(Context& ctx) noexcept;
    static void recreateSwapchain(Context& ctx) noexcept;
    
    static void createAttachments(Context& ctx) noexcept;
    static void destroyAttachments(Context& ctx) noexcept;
    
    static void createRenderPasses(Context& ctx) noexcept;
    static void destroyRenderPasses(Context& ctx) noexcept;

    static void createFramebuffers(Context& ctx) noexcept;
    static void destroyFramebuffers(Context& ctx) noexcept;

    static void createPipeline(Context& ctx) noexcept;
    static void destroyPipeline(Context& ctx) noexcept;

    static void createStaticMeshDescriptors(Context& ctx) noexcept;
    static void destroyStaticMeshDescriptors(Context& ctx) noexcept;
    
    static void createMeshBuffers(Context& ctx) noexcept;
    static void destroyMeshBuffers(Context& ctx) noexcept;

    // SIRenderer static interface
    static void prepareFrame(Context& ctx) noexcept;
    static void presentFrame(Context& ctx) noexcept;

    static void prepareRenderPass(Context& ctx) noexcept;
    static void presentRenderPass(Context& ctx) noexcept;

    static void prepareSwapchain(Context& ctx) noexcept;
    static void presentSwapchain(Context& ctx) noexcept;
};

using DefaultRenderer = Renderer<Vk, std::integral_constant<RendererType, RendererType::DEFAULT>>;

template <> [[nodiscard]] DefaultRenderer::Context setupRenderer<Vk, DefaultRenderer>(gapi::GApiContext<Vk>& ctx) noexcept;
template <> void teardownRenderer<Vk, DefaultRenderer>(DefaultRenderer::Context& ctx) noexcept;

}

#endif // RENDERER_VK_DEFAULT_RENDERER_HPP
