#pragma once

#include "handle.hpp"

namespace vk { namespace handles {

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
public:
    //  TO DO
    PhysicalDevice(const PhysicalDevice& other) = delete;
    PhysicalDevice(PhysicalDevice&& other);
    PhysicalDevice();
};

}}
