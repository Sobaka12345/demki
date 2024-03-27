#pragma once

#include "window.hpp"

#include <iopengl_window.hpp>

#include <string>

class GLFWwindow;

namespace shell::glfw {

class OpenGLWindow
    : public IOpenGLWindow
    , public Window
{
public:
    OpenGLWindow(int width, int height, std::string name);
    ~OpenGLWindow();

    virtual void swapBuffers() override;
    virtual renderer::IGraphicsContext& graphicsContext() override;

    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual void accept(renderer::RenderInfoVisitor& visitor) const override;

private:
    std::shared_ptr<renderer::ISwapchain> m_swapchain;
    std::shared_ptr<renderer::IGraphicsContext> m_graphicsContext;
};

}    //  namespace shell::glfw
