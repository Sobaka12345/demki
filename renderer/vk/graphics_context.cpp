#include "graphics_context.hpp"

#include "handles/surface.hpp"
#include "handles/memory.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"
#include "computer.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
#include "storage_buffer.hpp"

#include <operation_context.hpp>

#include <ivulkan_surface.hpp>
#include <iresources.hpp>

#include <cstring>
#include <iostream>

namespace renderer::vk {

static bool requiredExtensionsSupported(const std::vector<const char*>& required)
{
    uint32_t systemExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, nullptr);
    std::vector<VkExtensionProperties> systemExtensions(systemExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, systemExtensions.data());

    for (auto& required : required)
    {
        bool exists = false;
        for (auto& x : systemExtensions)
        {
            if (!std::strcmp(x.extensionName, required))
            {
                exists = true;
                break;
            }
        }

        if (exists == false)
        {
            return false;
        }
    }

    return true;
}

static bool requiredValidationLayerSupported(const std::span<const char* const> required)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto& required : required)
    {
        bool exists = false;
        for (auto& x : availableLayers)
        {
            if (!std::strcmp(x.layerName, required))
            {
                exists = true;
                break;
            }
        }

        if (exists == false)
        {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::string severity;
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: severity = "INFO: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: severity = "VERBOSE: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity = "WARNING: "; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: severity = "ERROR: "; break;
        default: severity = "WTF: ";
    }
    std::cout << severity << pCallbackData->messageIdNumber << ": " << pCallbackData->pMessageIdName
              << ":" << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

constexpr static auto s_debugMessengerCreateInfo =
    handles::DebugUtilsMessengerCreateInfoEXT()
        .messageSeverity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        .messageType(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        .pfnUserCallback(debugCallback);

#ifdef NDEBUG
const bool GraphicsContext::s_enableValidationLayers = false;
#else
const bool GraphicsContext::s_enableValidationLayers = true;
#endif

const std::vector<const char*> GraphicsContext::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

GraphicsContext::GraphicsContext(handles::InstanceCreateInfo instanceCreateInfo)
{
    if (s_enableValidationLayers)
    {
        ASSERT(requiredValidationLayerSupported(GraphicsContext::s_validationLayers),
            "required validation layers are absent");

        instanceCreateInfo.enabledLayerCount(s_validationLayers.size())
            .ppEnabledLayerNames(s_validationLayers.data())
            .pNext(&s_debugMessengerCreateInfo);
        //  we need pNext to debug vk(Create|Destroy)Instance
    }
    else
    {
        instanceCreateInfo.pNext(nullptr).enabledLayerCount(0);
    }

    ASSERT(Instance::create(vkCreateInstance, &instanceCreateInfo, nullptr) == VK_SUCCESS,
        "failed to create instance ;c");

    if (s_enableValidationLayers)
    {
        m_debugMessenger =
            std::make_unique<handles::DebugUtilsMessenger>(*this, s_debugMessengerCreateInfo);
    }
}

GraphicsContext::~GraphicsContext()
{
    m_buffers.clear();
    m_dynamicUniformShaderResources.clear();
    m_staticUniformShaderResources.clear();
    m_storageShaderResources.clear();
    m_device.reset();
    m_debugMessenger.reset();
}

void GraphicsContext::init(IVulkanSurface& surface)
{
    m_device = std::make_unique<handles::Device>(handle(), surface.surfaceKHR());
}

std::weak_ptr<vk::handles::Memory> GraphicsContext::fetchMemory(
	size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
{
	auto& newBuffer = m_buffers.emplaceBack(device(),
		handles::BufferCreateInfo().size(size).usage(usage).sharingMode(VK_SHARING_MODE_EXCLUSIVE));
	return newBuffer.allocateAndBindMemory(memoryProperties);
}

std::shared_ptr<ShaderInterfaceHandle> GraphicsContext::fetchHandleSpecific(ShaderBlockType sbt,
    uint32_t layoutSize)
{
    const uint32_t alignment = dynamicAlignment(layoutSize);

    auto insertAndFetchSpecificHandle = [&](auto& map) {
        if (auto el = map.find(layoutSize); el != map.end())
        {
            return ShaderInterfaceHandle::create(el->second);
        }
        using PairType = typename std::remove_reference<decltype(map)>::type::value_type;
        auto [iter, _] = map.emplace(
            PairType{ alignment, typename PairType::second_type{ device(), alignment, 100 } });

        return ShaderInterfaceHandle::create(iter->second);
    };

    if (sbt == ShaderBlockType::STORAGE)
    {
        return insertAndFetchSpecificHandle(m_storageShaderResources);
    }
    else if (sbt == ShaderBlockType::UNIFORM_DYNAMIC)
    {
        return insertAndFetchSpecificHandle(m_dynamicUniformShaderResources);
    }

    return insertAndFetchSpecificHandle(m_staticUniformShaderResources);
}

std::shared_ptr<IShaderInterfaceHandle> GraphicsContext::fetchHandle(ShaderBlockType sbt,
    uint32_t layoutSize)
{
    return fetchHandleSpecific(sbt, layoutSize);
}

VkFormat GraphicsContext::findSupportedFormat(const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice(), format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ASSERT(false, "failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
}

VkFormat GraphicsContext::findDepthFormat() const
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool GraphicsContext::hasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t GraphicsContext::dynamicAlignment(uint32_t layoutSize) const
{
    const uint32_t minAlignment =
        device().physicalDeviceProperties().limits.minUniformBufferOffsetAlignment;

    if (minAlignment > 0)
    {
        layoutSize = (layoutSize + minAlignment - 1) & ~(minAlignment - 1);
    }
    return layoutSize;
}

const handles::Device& GraphicsContext::device() const
{
    return *m_device;
}

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(IVulkanSurface& surface,
    ISwapchain::CreateInfo createInfo)
{
    return std::make_shared<Swapchain>(*this, surface, std::move(createInfo));
}

std::shared_ptr<IComputer> GraphicsContext::createComputer(IComputer::CreateInfo createInfo)
{
    return std::make_shared<Computer>(*this, std::move(createInfo));
}

std::shared_ptr<IComputePipeline> GraphicsContext::createComputePipeline(
    IComputePipeline::CreateInfo createInfo)
{
    return std::make_shared<ComputePipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IGraphicsPipeline> GraphicsContext::createGraphicsPipeline(
    IGraphicsPipeline::CreateInfo createInfo)
{
    return std::make_shared<GraphicsPipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IRenderer> GraphicsContext::createRenderer(IRenderer::CreateInfo createInfo)
{
    return std::make_shared<Renderer>(*this, std::move(createInfo));
}

std::shared_ptr<IStorageBuffer> GraphicsContext::createStorageBuffer(
    IStorageBuffer::CreateInfo createInfo)
{
    return std::make_shared<StorageBuffer>(*this, std::move(createInfo));
}

std::shared_ptr<IModel> GraphicsContext::createModel(std::filesystem::path path)
{
    return createModel(IModel::CreateInfo{ path });
}

std::shared_ptr<IModel> GraphicsContext::createModel(IModel::CreateInfo createInfo)
{
    return std::make_shared<Model>(*this, std::move(createInfo));
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(std::filesystem::path path)
{
    return createTexture(ITexture::CreateInfo{ path });
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(ITexture::CreateInfo createInfo)
{
    return std::make_shared<Texture>(*this, std::move(createInfo));
}

void GraphicsContext::waitIdle()
{
    m_device->waitIdle();
}

Multisampling GraphicsContext::maxSampleCount() const
{
    VkSampleCountFlags counts =
        m_device->physicalDeviceProperties().limits.framebufferColorSampleCounts &
        m_device->physicalDeviceProperties().limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
    {
        return Multisampling::MSA_64X;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT)
    {
        return Multisampling::MSA_32X;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT)
    {
        return Multisampling::MSA_16X;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT)
    {
        return Multisampling::MSA_8X;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT)
    {
        return Multisampling::MSA_4X;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT)
    {
        return Multisampling::MSA_2X;
    }

    return Multisampling::MSA_1X;
}

}    //  namespace renderer::vk
