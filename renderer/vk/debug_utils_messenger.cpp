#include "debug_utils_messenger.hpp"

#include "graphical_application.hpp"

namespace vk {

VkResult DebugUtilsMessenger::createMessenger(const GraphicalApplication& app, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(app, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DebugUtilsMessenger::destroyMessenger(const GraphicalApplication& app,
    VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(app, debugMessenger, pAllocator);
    }
}

DebugUtilsMessenger::DebugUtilsMessenger(const GraphicalApplication& app, DebugUtilsMessengerCreateInfoEXT createInfo)
    : Handle(nullptr)
    , m_app(app)
{
    ASSERT(create(createMessenger, app, &createInfo, nullptr) == VK_SUCCESS,
        "failed to set up debug messenger!");
}

DebugUtilsMessenger::~DebugUtilsMessenger()
{
    destroy(destroyMessenger, m_app, handle(), nullptr);
}

}