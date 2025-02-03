#pragma once

#include "handle.hpp"

#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(DebugUtilsMessengerCreateInfoEXT,
    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessengerCreateFlagsEXT, flags)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessageSeverityFlagsEXT, messageSeverity)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessageTypeFlagsEXT, messageType)
    VKSTRUCT_PROPERTY(PFN_vkDebugUtilsMessengerCallbackEXT, pfnUserCallback)
    VKSTRUCT_PROPERTY(void*, pUserData)
END_DECLARE_VKSTRUCT();

inline VkResult createMessenger(VkInstance app,
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

inline void destroyMessenger(VkInstance app,
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

template <typename T>
struct DebugUtilsMessengerEXTFunctions : public CRTPBase<T>
{
};

template <typename T>
struct DebugUtilsMessengerEXTGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE_FULL(DebugUtilsMessengerEXT,
    createMessenger,
    destroyMessenger,
    DebugUtilsMessengerEXTFunctions,
    DebugUtilsMessengerEXTGroupFunctions);
}

}    //  namespace renderer::vk::handles
