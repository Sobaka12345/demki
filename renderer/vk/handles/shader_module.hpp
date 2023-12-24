#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <filesystem>

namespace vk { namespace handles {

BEGIN_DECLARE_VKSTRUCT(ShaderModuleCreateInfo, VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkShaderModuleCreateFlags, flags)
    VKSTRUCT_PROPERTY(size_t, codeSize)
    VKSTRUCT_PROPERTY(const uint32_t*, pCode)
END_DECLARE_VKSTRUCT()

class Device;

class ShaderModule : public Handle<VkShaderModule>
{
    HANDLE(ShaderModule);

public:
    ShaderModule(const ShaderModule& other) = delete;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule(const Device& device, std::filesystem::path filePath) noexcept;
    ShaderModule(const Device& device, std::vector<char> code) noexcept;
    virtual ~ShaderModule();

protected:
    ShaderModule(
        const Device& device, ShaderModuleCreateInfo createInfo, VkHandleType* handlePtr) noexcept;
    ShaderModule(
        const Device& device, std::filesystem::path filePath, VkHandleType* handlePtr) noexcept;
    ShaderModule(const Device& device, std::vector<char> code, VkHandleType* handlePtr) noexcept;

private:
    const Device& m_device;
};

}}    //  namespace vk::handles
