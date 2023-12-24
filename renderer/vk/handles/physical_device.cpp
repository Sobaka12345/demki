#include "physical_device.hpp"

namespace vk { namespace handles {

PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : Handle(std::move(other))
{}

PhysicalDevice::PhysicalDevice() noexcept
    : Handle(static_cast<VkHandleType*>(nullptr))
{}

}}
