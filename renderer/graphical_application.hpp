#pragma once

#include "utils.hpp"

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

struct UniformBuffer
{
    VkBuffer buffer;
    void* mapped;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo descriptor;
};

class GraphicalApplication
{
public:
    int exec();

    void setWindowSize(int width, int height);
    void setApplicationName(std::string applicationName);
    GraphicalApplication();
    virtual ~GraphicalApplication();

protected:
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
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void recreateSwapChain();
    void cleanupSwapChain();
    void createWindowSurface();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) = 0;
    void drawFrame();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
private:
    std::string m_appName;
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;
    bool m_framebufferResized;
    bool m_windowIconified;

    vk::utils::QueueFamilyIndices m_queueFamilyIndices;

protected:
    uint8_t m_currentFrame;
    int m_maxFramesInFlight;

    VkInstance m_vkInstance;
    VkSurfaceKHR m_vkSurface;
    VkQueue m_vkPresentQueue;
    VkQueue m_vkGraphicsQueue;
    VkDevice m_vkLogicalDevice;
    VkPhysicalDevice m_vkPhysicalDevice;
    VkPhysicalDeviceLimits m_vkPhysicalDeviceLimits;
    VkDebugUtilsMessengerEXT m_vkDebugMessenger;
    VkExtent2D m_vkSwapChainExtent;
    VkCommandPool m_vkCommandPool;
    VkRenderPass m_vkRenderPass;
    std::vector<VkFramebuffer> m_vkSwapChainFramebuffers;

private:
    std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
    std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
    std::vector<VkFence> m_vkInFlightFences;

    std::vector<VkCommandBuffer> m_vkCommandBuffers;

    VkImage m_vkDepthImage;
    VkDeviceMemory m_vkDepthImageMemory;
    VkImageView m_vkDepthImageView;

    VkSwapchainKHR m_vkSwapChain;
    VkFormat m_vkSwapChainImageFormat;
    std::vector<VkImage> m_vkSwapChainImages;
    std::vector<VkImageView> m_vkSwapChainImageViews;
};
