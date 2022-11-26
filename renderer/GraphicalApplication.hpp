#pragma once
#include <set>
#include <string>
#include <vector>
#include <optional>

#if defined(_WIN32) || defined(WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__unix__)
    #define VK_USE_PLATFORM_XCB_KHR
    #define GLFW_EXPOSE_NATIVE_X11
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

struct GLFWwindow;

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
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    VkShaderModule createShaderModule(const std::vector<char>& code);
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
    VkDebugUtilsMessengerEXT m_vkDebugMessenger;

    VkPipeline m_vkPipeline;
    VkRenderPass m_vkRenderPass;
    VkPipelineLayout m_vkPipelineLayout;
    VkDescriptorSetLayout m_vkDescriptorSetLayout;

    std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
    std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
    std::vector<VkFence> m_vkInFlightFences;

    uint8_t m_currentFrame;

    VkBuffer m_vkVertexBuffer;
    VkDeviceMemory m_vkVertexBufferMemory;
    VkBuffer m_vkIndexBuffer;
    VkDeviceMemory m_vkIndexBufferMemory;
    std::vector<VkBuffer> m_vkUniformBuffers;
    std::vector<VkDeviceMemory> m_vkUniformBuffersMemory;
    std::vector<void*> m_vkUniformBuffersMapped;
    VkDescriptorPool m_vkDescriptorPool;
    std::vector<VkDescriptorSet> m_vkDescriptorSets;

    VkCommandPool m_vkCommandPool;
    std::vector<VkCommandBuffer> m_vkCommandBuffers;
    std::vector<VkFramebuffer> m_vkSwapChainFramebuffers;

    VkImage m_vkDepthImage;
    VkDeviceMemory m_vkDepthImageMemory;
    VkImageView m_vkDepthImageView;

    VkSwapchainKHR m_vkSwapChain;
    VkExtent2D m_vkSwapChainExtent;
    VkFormat m_vkSwapChainImageFormat;
    std::vector<VkImage> m_vkSwapChainImages;
    std::vector<VkImageView> m_vkSwapChainImageViews;
};
