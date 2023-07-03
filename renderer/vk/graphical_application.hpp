#pragma once

#include "handles/command.hpp"
#include "handles/device.hpp"
#include "handles/debug_utils_messenger.hpp"
#include "handles/fence.hpp"
#include "handles/framebuffer.hpp"
#include "handles/graphics_pipeline.hpp"
#include "handles/instance.hpp"
#include "handles/image.hpp"
#include "handles/image_view.hpp"
#include "handles/semaphore.hpp"


#include <set>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#if defined(_WIN32) || defined(WIN32)
#	define VK_USE_PLATFORM_WIN32_KHR
#	define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__unix__)
#	define VK_USE_PLATFORM_XCB_KHR
#	define GLFW_EXPOSE_NATIVE_X11
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace vk {

namespace handles {
class RenderPass;
}

class ResourceManager;

class GraphicalApplication : public handles::Instance
{
protected:
    using TimeResolution = std::nano;

public:
    const static bool s_enableValidationLayers;
    const static std::vector<const char*> s_validationLayers;

public:
    GraphicalApplication();
    GraphicalApplication(GraphicalApplication&& other) = delete;
    GraphicalApplication(const GraphicalApplication& other) = delete;
    virtual ~GraphicalApplication();

    int exec();

    void setWindowSize(int width, int height);
    void setApplicationName(std::string applicationName);

protected:
    ResourceManager& resources();

    static handles::ImageViewCreateInfo defaultImageView(
        VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static handles::GraphicsPipelineCreateInfo defaultGraphicsPipeline();

private:
    int mainLoop();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void windowIconifyCallback(GLFWwindow* window, int flag);

    virtual void initWindow();
    virtual void initBase();
    virtual void initApplication() = 0;

    void createInstance();
    void createSwapChain();
    void recreateSwapChain();
    void cleanupSwapChain();
    void createWindowSurface();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    virtual void update(int64_t dt) = 0;
    virtual void recordCommandBuffer(const handles::CommandBuffer& commandBuffer,
        const handles::Framebuffer& imageIndex) = 0;
    void drawFrame();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

protected:
    //  TO DO: WINDOW CLASS
    std::string m_appName;
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;
    bool m_framebufferResized;
    bool m_windowIconified;

    VkSurfaceKHR m_vkSurface;
    std::unique_ptr<handles::Device> m_device;
    VkExtent3D m_vkSwapChainExtent;
    std::unique_ptr<handles::RenderPass> m_renderPass;

private:
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<handles::DebugUtilsMessenger> m_debugMessenger;

    uint8_t m_currentFrame;
    int m_maxFramesInFlight;

    std::weak_ptr<handles::Queue> m_presentQueue;
    std::weak_ptr<handles::Queue> m_graphicsQueue;

    handles::HandleVector<handles::Semaphore> m_imageAvailableSemaphores;
    handles::HandleVector<handles::Semaphore> m_renderFinishedSemaphores;
    handles::HandleVector<handles::Fence> m_inFlightFences;

    handles::HandleVector<handles::CommandBuffer> m_commandBuffers;

    std::unique_ptr<handles::Image> m_depthImage;
    std::unique_ptr<handles::ImageView> m_depthImageView;

    VkSwapchainKHR m_vkSwapChain;
    VkFormat m_vkSwapChainImageFormat;
    handles::HandleVector<handles::Image> m_swapChainImages;
    handles::HandleVector<handles::ImageView> m_swapChainImageViews;
    handles::HandleVector<handles::Framebuffer> m_swapChainFramebuffers;
};

}    //  namespace vk
