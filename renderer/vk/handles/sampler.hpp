#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

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

template <typename T>
struct SamplerFunctions : public CRTPBase<T>
{
    CREATE_FUNC(Sampler, Device)
    DESTROY_FUNC(Sampler, Device)
};

template <typename T>
struct SamplerGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(Sampler, SamplerFunctions, SamplerGroupFunctions);
}

}    //  namespace renderer::vk
