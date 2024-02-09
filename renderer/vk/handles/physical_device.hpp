#pragma once

#include "handle.hpp"

namespace renderer::vk { namespace handles {

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
    HANDLE(PhysicalDevice);

public:
    //  TO DO
    PhysicalDevice(const PhysicalDevice& other) = delete;
    PhysicalDevice(PhysicalDevice&& other) noexcept;
    PhysicalDevice() noexcept;
};

}}
