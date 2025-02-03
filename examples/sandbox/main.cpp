#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/instrument.hpp>
#include <spirv-tools/optimizer.hpp>

#include <spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <fstream>
#include <filesystem>

#include "types.h"

#include "shaders_hpp/shader.vert.spv.hpp""

inline std::filesystem::path executablePath()
{
    static std::filesystem::path s_executablePath =
        []() -> std::filesystem::path {
#ifdef _WIN32
        wchar_t path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
#endif
    }()
                    .parent_path();

    return s_executablePath;
}

inline std::vector<uint32_t> readFile(std::filesystem::path filePath)
{
    if (filePath.is_relative())
    {
        filePath = executablePath() / filePath;
    }

    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    //  ASSERT(file.is_open(), ("failed to open file: " + filePath.string()).c_str());

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
    file.seekg(0);
    file.read((char*)(buffer.data()), fileSize);
    file.close();
    return buffer;
}

int main(int argc, char** argv)
{
    auto file = readFile(executablePath() / "shaders/shader.vert.spv");



    SpvReflectShaderModule module = {};
    SpvReflectResult result = spvReflectCreateShaderModule(sizeof(shader_vert_spv), shader_vert_spv, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    //  Go through each enumerate to examine it
    uint32_t count = 0;

    result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectDescriptorSet*> sets(count);
    result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    result = spvReflectEnumerateDescriptorBindings(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectDescriptorBinding*> bindings(count);
    result = spvReflectEnumerateDescriptorBindings(&module, &count, bindings.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    result = spvReflectEnumerateInterfaceVariables(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectInterfaceVariable*> interface_variables(count);
    result = spvReflectEnumerateInterfaceVariables(&module, &count, interface_variables.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    result = spvReflectEnumerateInputVariables(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectInterfaceVariable*> input_variables(count);
    result = spvReflectEnumerateInputVariables(&module, &count, input_variables.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    result = spvReflectEnumerateOutputVariables(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectInterfaceVariable*> output_variables(count);
    result = spvReflectEnumerateOutputVariables(&module, &count, output_variables.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    result = spvReflectEnumeratePushConstantBlocks(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    std::vector<SpvReflectBlockVariable*> push_constant(count);
    result = spvReflectEnumeratePushConstantBlocks(&module, &count, push_constant.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    //  Can set a breakpoint here and explorer the various variables enumerated.
    spvReflectDestroyShaderModule(&module);

    //  SpvReflectShaderModule module = {};
    //    SpvReflectResult result = spvReflectCreateShaderModule(file.size() * 4, file.data(),
    //    &module); assert(result == SPV_REFLECT_RESULT_SUCCESS);

    //  // //return Dummy{ argc, argv }.exec();
    //  // spirv_cross::CompilerGLSL glsl(readFile(executablePath()/"shaders/shader.vert.spv"));

    //  // // The SPIR-V is now parsed, and we can perform reflection on it.
    //  // spirv_cross::ShaderResources resources = glsl.get_shader_resources();

    //  // // Get all sampled images in the shader.
    //  // for (auto &resource : resources.storage_buffers)
    //  // {
    //  //     unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
    //  //     unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
    //  //     spirv_cross::SPIRType type = glsl.get_type(resource.type_id);
    //  //     auto name = glsl.get_name(resource.type_id);
    //  //     printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
    //  //     printf("%s ", name.c_str());

    //  //     // Modify the decoration to prepare it for GLSL.
    //  //     //glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);

    //  //     // Some arbitrary remapping if we want.
    //  //     //glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
    //  // }

    return 0;
}
