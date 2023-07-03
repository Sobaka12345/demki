#pragma once

#include "utils.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(ShaderModuleCreateInfo, VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO)
VKSTRUCT_PROPERTY(const void*              , pNext)
VKSTRUCT_PROPERTY(VkShaderModuleCreateFlags, flags)
VKSTRUCT_PROPERTY(size_t                   , codeSize)
VKSTRUCT_PROPERTY(const uint32_t*          , pCode)
END_DECLARE_VKSTRUCT()

class Device;

class ShaderModule : public Handle<VkShaderModule>
{
public:
    ShaderModule(const ShaderModule& other) = delete;
    ShaderModule(ShaderModule&& other);
    ShaderModule(const Device& device, std::filesystem::path createInfo, VkHandleType* handlePtr = nullptr);
    ShaderModule(const Device& device, std::vector<char> createInfo, VkHandleType* handlePtr = nullptr);
    ShaderModule(const Device& device, ShaderModuleCreateInfo createInfo, VkHandleType* handlePtr = nullptr);
    ~ShaderModule();

private:
    const Device& m_device;
};

}
