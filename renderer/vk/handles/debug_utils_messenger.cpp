#include "debug_utils_messenger.hpp"

#include "instance.hpp"

namespace renderer::vk { namespace handles {

VkResult DebugUtilsMessenger::createMessenger(const Instance& app,
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

void DebugUtilsMessenger::destroyMessenger(const Instance& app,
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

DebugUtilsMessenger::DebugUtilsMessenger(DebugUtilsMessenger&& other) noexcept
    : Handle(std::move(other))
    , m_app(other.m_app)
{}

DebugUtilsMessenger::DebugUtilsMessenger(const Instance& app,
    DebugUtilsMessengerCreateInfoEXT createInfo,
    VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_app(app)
{
    ASSERT(create(createMessenger, app, &createInfo, nullptr) == VK_SUCCESS,
        "failed to set up debug messenger!");
}

DebugUtilsMessenger::DebugUtilsMessenger(const Instance& app,
                                         DebugUtilsMessengerCreateInfoEXT createInfo) noexcept
    : DebugUtilsMessenger(app, std::move(createInfo), nullptr)
{}

DebugUtilsMessenger::~DebugUtilsMessenger()
{
    destroy(destroyMessenger, m_app, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles
