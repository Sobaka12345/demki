#pragma once

#include "assert.hpp"
#include "handle.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <set>
#include <span>
#include <vector>
#include <cstring>
#include <fstream>
#include <optional>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

namespace std {
    template<typename Type>
    struct is_std_array : std::false_type { };

    template<typename Item, std::size_t N>
    struct is_std_array< std::array<Item, N> > : std::true_type { };
}

#define COMMA ,

#define BEGIN_DECLARE_VKSTRUCT_IMPL(structName, vkStructName, sTypeVal)         \
struct structName : public VkStruct<vkStructName, sTypeVal>                     \
{                                                                               \
private:                                                                        \
    using Base = vkStructName;                                                  \
public:                                                                         \
    constexpr structName() noexcept                                             \
        : VkStruct<vkStructName, sTypeVal>(vkStructName{})                      \
    {}                                                                          \
                                                                                \
    constexpr structName(vkStructName sValue) noexcept                          \
        : VkStruct<vkStructName, sTypeVal>(std::move(sValue))                   \
    {                                                                           \
        static_assert(sizeof(structName) == sizeof(vkStructName));              \
    }

#define END_DECLARE_VKSTRUCT()                                                  \
};

#define BEGIN_DECLARE_VKSTRUCT(structName, sTypeVal)                            \
    BEGIN_DECLARE_VKSTRUCT_IMPL(structName, Vk##structName, sTypeVal)

#define BEGIN_DECLARE_UNTYPED_VKSTRUCT(structName)                              \
    BEGIN_DECLARE_VKSTRUCT(structName, VK_STRUCTURE_TYPE_MAX_ENUM)

#define VKSTRUCT_PROPERTY(type, name)                                           \
private:                                                                        \
    template <typename T>                                                       \
    constexpr inline void _set##name(T value)                                   \
    {                                                                           \
        if constexpr (std::is_std_array<T>::value)                              \
            std::copy(value.begin(), value.end(), Base::name);                  \
        else                                                                    \
            Base::name = value;                                                 \
    }                                                                           \
                                                                                \
public:                                                                         \
    template <typename T = type>                                                \
    constexpr T name() const                                                    \
    {                                                                           \
        if constexpr (std::is_std_array<T>::value){                             \
            return std::to_array<T::value_type>(Base::name);      }             \
        else                                                                    \
            return Base::name;                                                  \
    }                                                                           \
                                                                                \
    constexpr auto& name(type value)                                            \
    {                                                                           \
        _set##name(std::forward<type>(value));                                  \
        return *this;                                                           \
    }

namespace vk { namespace utils {

namespace fs {

inline std::filesystem::path getExePath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#endif
}

static std::filesystem::path s_executablePath = getExePath().parent_path();

inline std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(s_executablePath / filename, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(),
        ("failed to open file: " + (s_executablePath / filename).string()).c_str());

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

}

inline uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    ASSERT(false, "failed to find suitable memory type!");
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
}

inline bool requiredExtensionsSupported(const std::vector<const char*>& required)
{
    uint32_t systemExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, nullptr);
    std::vector<VkExtensionProperties> systemExtensions(systemExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &systemExtensionCount, systemExtensions.data());

    for(auto& required : required)
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

inline bool requiredValidationLayerSupported(const std::span<const char* const> required)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(auto& required : required)
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

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

inline SwapChainSupportDetails swapChainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

}

}
