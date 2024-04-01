#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace renderer::vk { namespace handles {

class Device;

BEGIN_DECLARE_VKSTRUCT(SamplerCreateInfo, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkSamplerCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkFilter, magFilter)
    VKSTRUCT_PROPERTY(VkFilter, minFilter)
    VKSTRUCT_PROPERTY(VkSamplerMipmapMode, mipmapMode)
    VKSTRUCT_PROPERTY(VkSamplerAddressMode, addressModeU)
    VKSTRUCT_PROPERTY(VkSamplerAddressMode, addressModeV)
    VKSTRUCT_PROPERTY(VkSamplerAddressMode, addressModeW)
    VKSTRUCT_PROPERTY(float, mipLodBias)
    VKSTRUCT_PROPERTY(VkBool32, anisotropyEnable)
    VKSTRUCT_PROPERTY(float, maxAnisotropy)
    VKSTRUCT_PROPERTY(VkBool32, compareEnable)
    VKSTRUCT_PROPERTY(VkCompareOp, compareOp)
    VKSTRUCT_PROPERTY(float, minLod)
    VKSTRUCT_PROPERTY(float, maxLod)
    VKSTRUCT_PROPERTY(VkBorderColor, borderColor)
    VKSTRUCT_PROPERTY(VkBool32, unnormalizedCoordinates)
END_DECLARE_VKSTRUCT()

class Sampler : public Handle<VkSampler>
{
    HANDLE(Sampler);

public:
	Sampler(const Sampler& other) = delete;
	Sampler(Sampler&& other) noexcept;
    Sampler(const Device& device, SamplerCreateInfo createInfo) noexcept;
    virtual ~Sampler();

protected:
    Sampler(const Device& device, SamplerCreateInfo createInfo, VkHandleType* handlePtr) noexcept;

private:
	const Device& m_device;
};

}}    //  namespace renderer::vk::handles
