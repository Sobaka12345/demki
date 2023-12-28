#include "graphics_context.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "computer.hpp"
#include "compute_pipeline.hpp"
#include "graphics_pipeline.hpp"
#include "renderer.hpp"

namespace ogl {

GraphicsContext::GraphicsContext(const Window& window)
    : m_window(window)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "failed to initialize opengl");
}

GraphicsContext::~GraphicsContext() {}

std::shared_ptr<IShaderInterfaceHandle> GraphicsContext::fetchHandle(ShaderBlockType sbt,
    uint32_t layoutSize)
{}

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

std::shared_ptr<ISwapchain> GraphicsContext::createSwapchain(ISwapchain::CreateInfo createInfo) {}

Multisampling GraphicsContext::maxSampleCount() const {}

void GraphicsContext::waitIdle() {}

const Window& GraphicsContext::window() const
{
    return m_window;
}

std::shared_ptr<IModel> GraphicsContext::createModel(IModel::CreateInfo createInfo) {}

std::shared_ptr<ITexture> GraphicsContext::createTexture(ITexture::CreateInfo createInfo) {}


}    //  namespace ogl
