#include "pipeline.hpp"

#include "shader_interface_handle.hpp"

namespace ogl {

GLenum toGLShaderType(IPipeline::ShaderType type)
{
    switch (type)
    {
        case ShaderStage::VERTEX: return GL_VERTEX_SHADER;
        case ShaderStage::FRAGMENT: return GL_FRAGMENT_SHADER;
        case ShaderStage::COMPUTE: return GL_COMPUTE_SHADER;
        default: ASSERT(false, "not implemented");
    }
    return -1;
}

void checkCompileErrors(GLuint shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "program")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            ASSERT(false, "shader compilation error of type: " + type + "\n" + infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            ASSERT(false, "program linking error error of type: " + type + "\n" + infoLog);
        }
    }
}

Pipeline::BindContext::BindContext(std::span<const uint32_t> bindingIndices)
    : bindingIndices(bindingIndices)
{}

void Pipeline::BindContext::bind(OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    static ShaderInterfaceHandle::TypeVisitor s_handleVisitor;

    auto uniforms = container.uniforms();
    for (size_t i = 0; i < uniforms.size(); ++i)
    {
        uniforms[i].handle.lock()->accept(s_handleVisitor);

        s_handleVisitor->bind(bindingIndices[i]);
    }
}

Pipeline::~Pipeline()
{
}

void Pipeline::init(const std::vector<InterfaceContainerInfo>& interfaceContainers,
    const std::vector<ShaderInfo>& shaders)
{
    m_shaderProgram = glCreateProgram();

    std::vector<GLuint> glShaders;
    for (auto& shader : shaders)
    {
        GLuint glShader = glShaders.emplace_back(glCreateShader(toGLShaderType(shader.type)));
        std::vector<char> shaderCode = readFile(shader.path);

        glShaderBinary(1, &glShader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderCode.data(),
            shaderCode.size());
        glSpecializeShader(glShader, "main", 0, nullptr, nullptr);
        checkCompileErrors(glShader, "shader");

        glAttachShader(m_shaderProgram, glShader);
    }

    glLinkProgram(m_shaderProgram);
    checkCompileErrors(m_shaderProgram, "program");

    for (auto glShader : glShaders)
    {
        glDeleteShader(glShader);
    }

    glUseProgram(m_shaderProgram);

    uint32_t bindingIndex = 0;
    for (auto& container : interfaceContainers)
    {
        for (auto& el : container.layout)
        {
            m_bindingIndices[container.id].push_back(bindingIndex++);
        }
    }
}

FragileSharedPtr<IPipelineBindContext> Pipeline::bindContext(
    const IShaderInterfaceContainer& container)
{
    auto [iter, emplaced] = m_bindContexts.emplace(
        container.typeId(), new BindContext{ m_bindingIndices[container.id()] });
    if (emplaced)
    {
        iter->second.setFragile(true);
    }
    
    return iter->second;
}

}    //  namespace ogl
