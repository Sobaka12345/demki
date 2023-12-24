#include "shader_module.hpp"

#include "device.hpp"

#include <utils.hpp>

namespace vk { namespace handles {

ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

ShaderModule::ShaderModule(
    const Device& device, ShaderModuleCreateInfo createInfo, VkHandleType* handlePtr) noexcept
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateShaderModule, m_device, &createInfo, nullptr) == VK_SUCCESS);
}

ShaderModule::ShaderModule(
    const Device& device, std::vector<char> code, VkHandleType* handlePtr) noexcept
    : ShaderModule(device,
          ShaderModuleCreateInfo()
              .codeSize(code.size())
              .pCode(reinterpret_cast<const uint32_t*>(code.data())),
          handlePtr)
{}

ShaderModule::ShaderModule(
    const Device& device, std::filesystem::path filePath, VkHandleType* handlePtr) noexcept
    : ShaderModule(device, readFile(filePath), handlePtr)
{}

ShaderModule::ShaderModule(const Device& device, std::vector<char> code) noexcept
    : ShaderModule(device,
          ShaderModuleCreateInfo()
              .codeSize(code.size())
              .pCode(reinterpret_cast<const uint32_t*>(code.data())),
          nullptr)
{}

ShaderModule::ShaderModule(const Device& device, std::filesystem::path path) noexcept
    : ShaderModule(device, readFile(path))
{}

ShaderModule::~ShaderModule()
{
    destroy(vkDestroyShaderModule, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
