#include "physical_device.hpp"

namespace vk { namespace handles {

PhysicalDevice::PhysicalDevice(PhysicalDevice &&other)
    : Handle(std::move(other))
{}

PhysicalDevice::PhysicalDevice()
    : Handle(nullptr)
{}

}}
