#include "graphics_pipeline.hpp"

#include "shader_interface_handle.hpp"

#include <operation_context.hpp>
#include <utils.hpp>

namespace ogl {

float toGLSampleShadingCoefficient(IGraphicsPipeline::CreateInfo::SampleShading sampleShading)
{
    switch (sampleShading)
    {
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_0_PERCENT: return 0.0f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_20_PERCENT: return 0.2f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_40_PERCENT: return 0.4f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_60_PERCENT: return 0.6f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_80_PERCENT: return 0.8f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_100_PERCENT: return 1.0f;
    }

    ASSERT(false, "sample shading not declared");
    return 0.0f;
}

int toGLFrontFace(IGraphicsPipeline::CreateInfo::FrontFace frontFace)
{
    switch (frontFace)
    {
        case IGraphicsPipeline::CreateInfo::CLOCKWISE: return GL_CW;
        case IGraphicsPipeline::CreateInfo::COUNTER_CLOCKWISE: return GL_CCW;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLCullMode(IGraphicsPipeline::CreateInfo::CullMode cullMode)
{
    switch (cullMode)
    {
        case IGraphicsPipeline::CreateInfo::BACK: return GL_BACK;
        case IGraphicsPipeline::CreateInfo::FRONT: return GL_FRONT;
        case IGraphicsPipeline::CreateInfo::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLPolygonMode(IGraphicsPipeline::CreateInfo::PolygonMode polygonMode)
{
    switch (polygonMode)
    {
        case IGraphicsPipeline::CreateInfo::POINT: return GL_POINT;
        case IGraphicsPipeline::CreateInfo::LINE: return GL_LINE;
        case IGraphicsPipeline::CreateInfo::FILL: return GL_FILL;
    }

    ASSERT(false, "not implemented");
    return -1;
}

int toGLTopology(IGraphicsPipeline::CreateInfo::PrimitiveTopology topology)
{
    switch (topology)
    {
        case IGraphicsPipeline::CreateInfo::LINES: return GL_LINES;
        case IGraphicsPipeline::CreateInfo::TRIANGLES: return GL_TRIANGLES;
        case IGraphicsPipeline::CreateInfo::POINTS: return GL_POINTS;
    }

    ASSERT(false, "not implemented");
    return -1;
}

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

GraphicsPipeline::BindContext::BindContext(std::span<const uint32_t> bindingIndices)
    : bindingIndices(bindingIndices)
{}

void GraphicsPipeline::BindContext::bind(::OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    static ShaderInterfaceHandle::TypeVisitor s_handleVisitor;

    auto uniforms = container.uniforms();
    for (size_t i = 0; i < uniforms.size(); ++i)
    {
        uniforms[i].handle.lock()->accept(s_handleVisitor);

        if (uniforms[i].binding.type == ShaderBlockType::SAMPLER)
        {
            auto val = GL_TEXTURE0 + bindingIndices[i];
            glActiveTexture(val);
            glBindTexture(GL_TEXTURE_2D, s_handleVisitor->texture);
        }
        else
        {
            glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndices[i], s_handleVisitor->buffer, 0,
                s_handleVisitor->size);
        }
    }
}

GraphicsPipeline::GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_primitiveTopology(toGLTopology(createInfo.primitiveTopology()))
    , m_cullMode(toGLCullMode(createInfo.cullMode()))
    , m_polygonMode(toGLPolygonMode(createInfo.polygonMode()))
    , m_frontFace(toGLFrontFace(createInfo.frontFace()))
    , m_sampleShading(toGLSampleShadingCoefficient(createInfo.sampleShading()))
{
    m_shaderProgram = glCreateProgram();

    std::vector<GLuint> glShaders;
    for (auto& shader : createInfo.shaders())
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
    for (auto& container : createInfo.interfaceContainers())
    {
        for (auto& el : container.layout)
        {
            m_bindingIndices[container.id].push_back(bindingIndex++);
        }
    }
}

GraphicsPipeline::~GraphicsPipeline()
{
    m_isInDestruction = true;
}

int GraphicsPipeline::primitiveTopology() const
{
    return m_primitiveTopology;
}

void GraphicsPipeline::bind(::OperationContext& context)
{
    get(context).graphicsPipeline = this;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (m_sampleShading > 0.01f)
    {
        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(m_sampleShading);
    }
    else
    {
        glDisable(GL_SAMPLE_SHADING);
    }

    glEnable(GL_CULL_FACE);
    glCullFace(m_cullMode);
    glFrontFace(m_frontFace);
    glPolygonMode(m_cullMode, m_polygonMode);

    glUseProgram(m_shaderProgram);
}

FragileSharedPtr<IPipelineBindContext> GraphicsPipeline::bindContext(
    const IShaderInterfaceContainer& container)
{
    const auto containerTypeId = container.typeId();

    auto [contextIter, _] = m_bindContexts.emplace(containerTypeId,
        new BindContext{ m_bindingIndices[container.id()] });

    contextIter->second.registerDeleteCallback([&, containerTypeId](BindContext* context) {
        if (!m_isInDestruction) m_bindContexts.erase(containerTypeId);
    });

    return contextIter->second;
}

}    //  namespace ogl
