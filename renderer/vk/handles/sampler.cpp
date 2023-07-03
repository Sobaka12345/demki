#include "sampler.hpp"

#include "device.hpp"

namespace vk { namespace handles {

Sampler::Sampler(Sampler&& other) noexcept
	: Handle(std::move(other))
	, m_device(other.m_device)
{}

Sampler::Sampler(const Device& device, SamplerCreateInfo createInfo, VkHandleType* handlePtr)
	: Handle(handlePtr)
	, m_device(device)
{
	ASSERT(create(vkCreateSampler, device, &createInfo, nullptr) == VK_SUCCESS);
}

Sampler::~Sampler()
{
	destroy(vkDestroySampler, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
