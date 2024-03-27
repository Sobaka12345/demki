#pragma once

#include "handle.hpp"

#include "../utils.hpp"

namespace renderer::vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(DebugUtilsMessengerCreateInfoEXT,
    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessengerCreateFlagsEXT, flags)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessageSeverityFlagsEXT, messageSeverity)
    VKSTRUCT_PROPERTY(VkDebugUtilsMessageTypeFlagsEXT, messageType)
    VKSTRUCT_PROPERTY(PFN_vkDebugUtilsMessengerCallbackEXT, pfnUserCallback)
    VKSTRUCT_PROPERTY(void*, pUserData)
END_DECLARE_VKSTRUCT();

class Instance;

class DebugUtilsMessenger : public Handle<VkDebugUtilsMessengerEXT>
{
    HANDLE(DebugUtilsMessenger);

public:
	DebugUtilsMessenger(const DebugUtilsMessenger& other) = delete;
    DebugUtilsMessenger(DebugUtilsMessenger&& other) noexcept;
    DebugUtilsMessenger(const Instance& app, DebugUtilsMessengerCreateInfoEXT createInfo) noexcept;
	~DebugUtilsMessenger();

protected:
    DebugUtilsMessenger(const Instance& app,
        DebugUtilsMessengerCreateInfoEXT createInfo,
        VkHandleType* handlePtr) noexcept;

private:
    static VkResult createMessenger(const Instance& app,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void destroyMessenger(const Instance& app,
        VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

private:
    const Instance& m_app;
};

}}    //  namespace renderer::vk::handles
