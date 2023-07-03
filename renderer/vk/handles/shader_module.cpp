#include "shader_module.hpp"

#include "device.hpp"

namespace vk { namespace handles {

ShaderModule::ShaderModule(ShaderModule &&other)
    : Handle(std::move(other))
    , m_device(other.m_device)
{}

ShaderModule::ShaderModule(
    const Device &device, std::filesystem::path path, VkHandleType *handlePtr)
    : ShaderModule(device, utils::fs::readFile(path), handlePtr)
{}

ShaderModule::ShaderModule(const Device &device, std::vector<char> code, VkHandleType *handlePtr)
    : ShaderModule(device,
          ShaderModuleCreateInfo()
              .codeSize(code.size())
              .pCode(reinterpret_cast<const uint32_t *>(code.data())),
          handlePtr)
{}

ShaderModule::ShaderModule(
    const Device &device, ShaderModuleCreateInfo createInfo, VkHandleType *handlePtr)
    : Handle(handlePtr)
    , m_device(device)
{
    ASSERT(create(vkCreateShaderModule, m_device, &createInfo, nullptr) == VK_SUCCESS);
}

ShaderModule::~ShaderModule()
{
    destroy(vkDestroyShaderModule, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
