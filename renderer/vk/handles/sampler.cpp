#include "sampler.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {

Sampler::Sampler(Sampler&& other) noexcept
	: Handle(std::move(other))
    , m_device(other.m_device)
{}

Sampler::Sampler(
    const Device& device, SamplerCreateInfo createInfo, VkHandleType* handlePtr) noexcept
	: Handle(handlePtr)
	, m_device(device)
{
	ASSERT(create(vkCreateSampler, device, &createInfo, nullptr) == VK_SUCCESS);
}

Sampler::Sampler(const Device& device, SamplerCreateInfo createInfo) noexcept
    : Sampler(device, std::move(createInfo), nullptr)
{}

Sampler::~Sampler()
{
	destroy(vkDestroySampler, m_device, handle(), nullptr);
}

}}    //  namespace renderer::vk::handles
