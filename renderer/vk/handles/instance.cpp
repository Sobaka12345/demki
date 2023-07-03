#include "instance.hpp"

namespace vk { namespace handles {

Instance::Instance(InstanceCreateInfo createInfo)
    : Handle(nullptr)
{
    ASSERT(create(vkCreateInstance, &createInfo, nullptr) == VK_SUCCESS,
        "failed to create instance!");
}

Instance::Instance()
    : Handle(nullptr)
{}

Instance::~Instance()
{
    destroy(vkDestroyInstance, handle(), nullptr);
}

}}    //  namespace vk::handles
