#include "graphics_context.hpp"

#include "computer.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "storage_buffer.hpp"
#include "uniform_buffer.hpp"
#include "texture.hpp"

namespace renderer::ogl {


void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH)
    {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

GraphicsContext::GraphicsContext(IOpenGLSurface& defaultSurface)
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}

GraphicsContext::~GraphicsContext() {}

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(IOpenGLSurface& surface,
    ISwapchain::CreateInfo createInfo)
{
    return std::make_shared<Swapchain>(*this, surface, std::move(createInfo));
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
{
    return std::make_shared<StorageBuffer>(*this, std::move(createInfo));
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

std::shared_ptr<IMesh> GraphicsContext::createMesh(std::filesystem::path path)
{
    return createMesh(IMesh::CreateInfo{ path });
}

std::shared_ptr<IMesh> GraphicsContext::createMesh(IMesh::CreateInfo createInfo)
{
    return std::make_shared<Mesh>(*this, std::move(createInfo));
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(std::filesystem::path path)
{
    return createTexture(ITexture::CreateInfo{ path });
}

std::shared_ptr<ITexture> GraphicsContext::createTexture(ITexture::CreateInfo createInfo)
{
    return std::make_shared<Texture>(*this, std::move(createInfo));
}

std::shared_ptr<IUniformBuffer> GraphicsContext::createUniformBuffer(
    IUniformBuffer::CreateInfo createInfo)
{
    return std::make_shared<UniformBuffer>(*this, std::move(createInfo));
}


}    //  namespace renderer::ogl
