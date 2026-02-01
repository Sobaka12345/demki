#include "application.hpp"
#include "assert.hpp"

#include <cstring>
#include <span>
#include <vulkan/vulkan_core.h>

namespace shell::abstract::vk {

const std::vector<const char*> Application::s_validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

static VkResult createDebugMessenger(VkInstance app,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(app, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void destroyDebugMessenger(VkInstance app,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(app, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT /* messageType */,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /* pUserData */)
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

const VkDebugUtilsMessengerCreateInfoEXT Application::s_debugMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
    };


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

Application::Application(int& /* argc */, char **/* argv */) noexcept
    : m_instance(VK_NULL_HANDLE)
    , m_debugUtilsMessenger(VK_NULL_HANDLE)
{}

Application::~Application()
{
    if (m_debugUtilsMessenger != VK_NULL_HANDLE) {
        destroyDebugMessenger(m_instance, m_debugUtilsMessenger, nullptr);
    }
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
    }
}

void Application::init() noexcept
{

}

Application::VkInstanceBuilder::VkInstanceBuilder(Application& app) noexcept
    : app(app)
{
    sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    pNext = nullptr;
    pApplicationInfo = nullptr;
    ppEnabledExtensionNames = nullptr;
    enabledLayerCount= 0;
    ppEnabledLayerNames = nullptr;
    enabledExtensionCount = 0;
}

VkInstance Application::VkInstanceBuilder::build() noexcept {
    if constexpr (s_validationEnabled) {
        {
            static std::vector<const char*> s_extensions;
            s_extensions = {ppEnabledExtensionNames,ppEnabledExtensionNames + enabledExtensionCount};
            s_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            enabledExtensionCount = s_extensions.size();
            ppEnabledExtensionNames = s_extensions.data();
        }

        {
            ASSERT(requiredValidationLayerSupported(Application::s_validationLayers),
                "required validation layers are absent");

            static std::vector<const char*> s_layers;
            s_layers = {ppEnabledLayerNames, ppEnabledLayerNames + enabledLayerCount};
            std::copy(s_validationLayers.begin(), s_validationLayers.end(), std::back_inserter(s_layers));
            enabledLayerCount = s_layers.size();
            ppEnabledLayerNames = s_layers.data();
            
            if (!pNext)
                pNext = &s_debugMessengerCreateInfo;
        }
    }

    if (!pApplicationInfo) {
        static VkApplicationInfo defaultAppInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Application",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Demki",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_1
        };
        pApplicationInfo = &defaultAppInfo;
    }

    VkInstance result;
    ASSERT(VK_SUCCESS == vkCreateInstance(this, nullptr, &result));
    app.m_instance = result;
    if constexpr (s_validationEnabled) {
        VkDebugUtilsMessengerEXT debugMessenger;
        createDebugMessenger(result, &s_debugMessengerCreateInfo, nullptr, &debugMessenger);
        app.m_debugUtilsMessenger = debugMessenger;
    }

    return result;
}

Application::VkInstanceBuilder& Application::vkInstanceBuilder() noexcept {
    static Application::VkInstanceBuilder s_result (*this);
    return s_result;
}

}