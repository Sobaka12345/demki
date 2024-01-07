#include "graphics_context.hpp"

#include "computer.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "shader_interface_handle.hpp"
#include "texture.hpp"

#include <window.hpp>

#include <GLFW/glfw3.h>

namespace ogl {

GLenum memoryUsage(ShaderBlockType sbt)
{
    switch (sbt)
    {
        case ShaderBlockType::UNIFORM_STATIC: return GL_STATIC_DRAW;
        case ShaderBlockType::UNIFORM_DYNAMIC: return GL_DYNAMIC_DRAW;
        default: ASSERT(false, "not implemented");
    }

    return GL_INVALID_ENUM;
}

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

GraphicsContext::GraphicsContext(Window& window, Resources& resources)
    : IGraphicsContext(window, GAPI::OpenGL)
    , m_window(window)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to initialize opengl");

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}

GraphicsContext::~GraphicsContext() {}

std::shared_ptr<IShaderInterfaceHandle> GraphicsContext::fetchHandle(ShaderBlockType sbt,
    uint32_t layoutSize)
{
    return std::make_shared<ShaderInterfaceHandle>(layoutSize, memoryUsage(sbt));
}

std::shared_ptr<IComputer> GraphicsContext::createComputer(IComputer::CreateInfo createInfo)
{
    return std::make_shared<Computer>(*this, std::move(createInfo));
}

std::shared_ptr<IComputePipeline> GraphicsContext::createComputePipeline(
    IComputePipeline::CreateInfo createInfo)
{
    return std::make_shared<ComputePipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IGraphicsPipeline> GraphicsContext::createGraphicsPipeline(
    IGraphicsPipeline::CreateInfo createInfo)
{
    return std::make_shared<GraphicsPipeline>(*this, std::move(createInfo));
}

std::shared_ptr<IRenderer> GraphicsContext::createRenderer(IRenderer::CreateInfo createInfo)
{
    return std::make_shared<Renderer>(*this, std::move(createInfo));
}

std::shared_ptr<IStorageBuffer> GraphicsContext::createStorageBuffer(
    IStorageBuffer::CreateInfo createInfo)
{}

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(ISwapchain::CreateInfo createInfo)
{
    return std::make_shared<Swapchain>(*this, std::move(createInfo));
}

Multisampling GraphicsContext::maxSampleCount() const
{
    static const GLint value = []() {
        GLint result;
        glGetIntegerv(GL_MAX_SAMPLES, &result);
        return result;
    }();

    return static_cast<Multisampling>(value);
}

void GraphicsContext::waitIdle() {}

const Window& GraphicsContext::window() const
{
    return m_window;
}

std::shared_ptr<IModel> GraphicsContext::createModel(std::filesystem::path path)
{
    return createModel(IModel::CreateInfo{ path });
}

std::shared_ptr<IModel> GraphicsContext::createModel(IModel::CreateInfo createInfo)
{
    return std::make_shared<Model>(*this, std::move(createInfo));
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(std::filesystem::path path)
{
    return createTexture(ITexture::CreateInfo{ path });
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(ITexture::CreateInfo createInfo)
{
    return std::make_shared<Texture>(*this, std::move(createInfo));
}


}    //  namespace ogl
