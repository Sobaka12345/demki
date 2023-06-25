#pragma once

#include "buffer.hpp"
#include "command.hpp"
#include "command_pool.hpp"
#include "creators.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "image.hpp"
#include "image_view.hpp"

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#if defined(_WIN32) || defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__unix__)
    #define VK_USE_PLATFORM_XCB_KHR
    #define GLFW_EXPOSE_NATIVE_X11
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class GraphicalApplication
{
public:
    using TimeResolution = std::nano;
    const static bool s_enableValidationLayers;
    const static std::vector<const char*> s_validationLayers;

public:
    int exec();

    void setWindowSize(int width, int height);
    void setApplicationName(std::string applicationName);
    GraphicalApplication();
    virtual ~GraphicalApplication();

protected:
    class UpdateTimer
    {
    public:
        UpdateTimer(int32_t interval = TimeResolution::den)
            : m_interval(interval)
            , m_timer(0)
            , m_speedUpCoeff(1.0f)
        {}

        bool timePassed(int64_t dt)
        {
            return (m_timer += dt) > m_interval / m_speedUpCoeff ? m_timer = 0, true : false;
        }

        template <typename ResolutionType = TimeResolution>
        int64_t interval() const
        {
            return (m_interval / double(TimeResolution::den)) * ResolutionType::den;
        }

        void setNormalSpeed()
        {
            m_speedUpCoeff = 1.0f;
        }

        void setSpeedUp(float coeff)
        {
            m_speedUpCoeff = coeff;
        }

        void setIntervalMS(int64_t intervalMS)
        {
            setInterval<std::milli>(intervalMS);
        }

        template <typename ResolutionType = TimeResolution>
        void setInterval(int64_t interval)
        {
            m_interval = (interval / double(ResolutionType::den)) * TimeResolution::den;
        }

        template <typename ResolutionType = TimeResolution>
        void reset(int64_t initialDelay = 0)
        {
            m_timer = -(initialDelay / double(ResolutionType::den)) * TimeResolution::den;
        }

    private:
        int64_t m_timer;
        int64_t m_interval;
        float m_speedUpCoeff;
    };


protected:
    static vk::ImageViewCreateInfo defaultImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static VkPipeline defaultGraphicsPipeline(VkDevice device, VkRenderPass renderPass,
        VkPipelineLayout pipelineLayout, VkShaderModule vertexShader, VkShaderModule fragmentShader);

private:
    int mainLoop();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void windowIconifyCallback(GLFWwindow* window, int flag);

    virtual void initWindow();
    virtual void initBase();
    virtual void initApplication() = 0;

    // Vulkan
    void createInstance();
    void setupDebugMessenger();
    void createSwapChain();
    void recreateSwapChain();
    void cleanupSwapChain();
    void createWindowSurface();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    virtual void update(int64_t dt) = 0;
    virtual void recordCommandBuffer(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex) = 0;
    void drawFrame();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

// TO DO: WINDOW CLASS
protected:
    std::string m_appName;
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;
    bool m_framebufferResized;
    bool m_windowIconified;

protected:
    template <template <typename> class BufferType, class BufferData>
    inline BufferType<BufferData>* createAndWriteGPUBuffer(const std::span<const BufferData> data)
    {
        BufferType<BufferData>* buffer = new BufferType<BufferData>(*m_device, data);
        buffer->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vk::StagingBuffer stagingBuffer(*m_device, buffer->size());
        stagingBuffer.allocateAndBindMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .lock()->map()
            .lock()->write(data.data(), buffer->size());
        stagingBuffer.memory().lock()->unmap();

        stagingBuffer.copyTo(*buffer, vk::create::bufferCopy(buffer->size()));

        return buffer;
    }

protected:
    uint8_t m_currentFrame;
    int m_maxFramesInFlight;

    VkInstance m_vkInstance;
    VkSurfaceKHR m_vkSurface;
    std::weak_ptr<vk::Queue> m_presentQueue;
    std::weak_ptr<vk::Queue> m_graphicsQueue;
    std::unique_ptr<vk::Device> m_device;
    VkDebugUtilsMessengerEXT m_vkDebugMessenger;
    VkExtent3D m_vkSwapChainExtent;
    VkRenderPass m_vkRenderPass;
    std::vector<vk::Framebuffer> m_swapChainFramebuffers;

private:
    std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
    std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
    std::vector<VkFence> m_vkInFlightFences;

    vk::HandleVector<vk::CommandBuffer> m_commandBuffers;

    std::unique_ptr<vk::Image> m_depthImage;
    std::unique_ptr<vk::ImageView> m_depthImageView;

    VkSwapchainKHR m_vkSwapChain;
    VkFormat m_vkSwapChainImageFormat;
    vk::HandleVector<vk::Image> m_swapChainImages;
    vk::HandleVector<vk::ImageView> m_swapChainImageViews;
};
