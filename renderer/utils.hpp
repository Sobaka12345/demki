#pragma once

#include <vector>
#include <cassert>
#include <cstring>
#include <fstream>
#include <filesystem>

#include <vulkan/vulkan.h>

#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

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

inline std::vector<char> readFile(const std::string& filename)
{
    static std::filesystem::path executablePath = getExePath().parent_path();
    std::ifstream file(executablePath / filename, std::ios::ate | std::ios::binary);

    assert(file.is_open() &&
        ("failed to open file: " + (executablePath / filename).string()).c_str());

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

    assert("failed to find suitable memory type!");
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

inline bool requiredValidationLayerSupported(const std::vector<const char*>& required)
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

}

}
