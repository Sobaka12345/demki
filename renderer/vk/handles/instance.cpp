#include "instance.hpp"

namespace renderer::vk { namespace handles {

Instance::Instance() noexcept
    : Handle(static_cast<VkHandleType*>(nullptr))
{}

Instance::Instance(InstanceCreateInfo createInfo) noexcept
    : Instance()
{
    ASSERT(create(vkCreateInstance, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create instance!");
}

Instance::~Instance()
{
    destroy(vkDestroyInstance, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles
