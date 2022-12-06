#pragma once
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

struct GLFWwindow;
namespace vk {
class Buffer;
}

struct UniformBuffer
{
    VkBuffer buffer;
    void* mapped;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo descriptor;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    std::set<uint32_t> uniqueQueueFamilies() const
    {
        if (isComplete())
        {
            return {
                presentFamily.value(),
                graphicsFamily.value(),
            };
        }
        return {};
    }

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class GraphicalApplication
{
public:
    static GraphicalApplication& instance();

    int exec();

    void setWindowSize(int width, int height);
    void setApplicationName(std::string applicationName);

private:
    GraphicalApplication();
    ~GraphicalApplication();

    int mainLoop();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void windowIconifyCallback(GLFWwindow* window, int flag);
    void initWindow();
    void initVulkan();

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
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    void updateUniformBuffer(uint32_t currentImage);
    void updateDynUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
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
    int m_maxFramesInFlight;
    bool m_framebufferResized;
    bool m_windowIconified;

    QueueFamilyIndices m_queueFamilyIndices;

    VkInstance m_vkInstance;
    VkSurfaceKHR m_vkSurface;
    VkQueue m_vkPresentQueue;
    VkQueue m_vkGraphicsQueue;
    VkDevice m_vkLogicalDevice;
    VkPhysicalDevice m_vkPhysicalDevice;
    VkPhysicalDeviceLimits m_vkPhysicalDeviceLimits;
    VkDebugUtilsMessengerEXT m_vkDebugMessenger;

    VkPipeline m_vkPipeline;
    VkRenderPass m_vkRenderPass;
    VkPipelineLayout m_vkPipelineLayout;
    VkDescriptorSetLayout m_vkDescriptorSetLayout;

    std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
    std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
    std::vector<VkFence> m_vkInFlightFences;

    uint8_t m_currentFrame;

    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    VkBuffer m_vkIndexBuffer;
    VkDeviceMemory m_vkIndexBufferMemory;
    VkDescriptorPool m_vkDescriptorPool;
    std::vector<VkDescriptorSet> m_vkDescriptorSets;

    VkCommandPool m_vkCommandPool;
    std::vector<VkCommandBuffer> m_vkCommandBuffers;
    std::vector<VkFramebuffer> m_vkSwapChainFramebuffers;

    VkImage m_vkDepthImage;
    VkDeviceMemory m_vkDepthImageMemory;
    VkImageView m_vkDepthImageView;

    struct UniformBuffers
    {
        UniformBuffer m_viewProjectionBuffer;
        UniformBuffer m_modelBufffer;
    };
    uint32_t m_dynamicAlignment;
    std::vector<UniformBuffers> m_uniformBuffers;
    std::vector<glm::mat4x4*> m_modelBuffers;

    VkSwapchainKHR m_vkSwapChain;
    VkExtent2D m_vkSwapChainExtent;
    VkFormat m_vkSwapChainImageFormat;
    std::vector<VkImage> m_vkSwapChainImages;
    std::vector<VkImageView> m_vkSwapChainImageViews;
};
