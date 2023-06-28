#pragma once

#include "utils.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(DebugUtilsMessengerCreateInfoEXT, VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
VKSTRUCT_PROPERTY(const void*, pNext)
VKSTRUCT_PROPERTY(VkDebugUtilsMessengerCreateFlagsEXT, flags)
VKSTRUCT_PROPERTY(VkDebugUtilsMessageSeverityFlagsEXT, messageSeverity)
VKSTRUCT_PROPERTY(VkDebugUtilsMessageTypeFlagsEXT, messageType)
VKSTRUCT_PROPERTY(PFN_vkDebugUtilsMessengerCallbackEXT, pfnUserCallback)
VKSTRUCT_PROPERTY(void*, pUserData)
END_DECLARE_VKSTRUCT();

class GraphicalApplication;

class DebugUtilsMessenger : public Handle<VkDebugUtilsMessengerEXT>
{
public:
	DebugUtilsMessenger(DebugUtilsMessenger&& other) = delete;
	DebugUtilsMessenger(const DebugUtilsMessenger& other) = delete;
	DebugUtilsMessenger(const GraphicalApplication& app, DebugUtilsMessengerCreateInfoEXT createInfo);
	~DebugUtilsMessenger();

private:
	static VkResult createMessenger(const GraphicalApplication& app, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void destroyMessenger(const GraphicalApplication& app, VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

private:
	const GraphicalApplication& m_app;
};

}