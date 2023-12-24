#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(ApplicationInfo, VK_STRUCTURE_TYPE_APPLICATION_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(const char*, pApplicationName)
    VKSTRUCT_PROPERTY(uint32_t, applicationVersion)
    VKSTRUCT_PROPERTY(const char*, pEngineName)
    VKSTRUCT_PROPERTY(uint32_t, engineVersion)
    VKSTRUCT_PROPERTY(uint32_t, apiVersion)
END_DECLARE_VKSTRUCT();

BEGIN_DECLARE_VKSTRUCT(InstanceCreateInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkInstanceCreateFlags, flags)
    VKSTRUCT_PROPERTY(const VkApplicationInfo*, pApplicationInfo)
    VKSTRUCT_PROPERTY(uint32_t, enabledLayerCount)
    VKSTRUCT_PROPERTY(const char* const*, ppEnabledLayerNames)
    VKSTRUCT_PROPERTY(uint32_t, enabledExtensionCount)
    VKSTRUCT_PROPERTY(const char* const*, ppEnabledExtensionNames)
END_DECLARE_VKSTRUCT();

class Instance : public Handle<VkInstance>
{
    HANDLE(Instance);

public:
    Instance(const Instance& other) = delete;
    Instance(Instance&& other) = delete;
    Instance(InstanceCreateInfo createInfo) noexcept;
    virtual ~Instance();

protected:
    Instance() noexcept;
};

}}    //  namespace vk::handles
