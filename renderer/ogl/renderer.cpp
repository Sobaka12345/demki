#include "renderer.hpp"

#include "ispecific_operation_target.hpp"

#include "graphics_context.hpp"

#include <irender_target.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

Renderer::Renderer(const GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
    , m_clearColor(createInfo.clearValue)
    , m_multisampling(createInfo.multisampling)
{}

renderer::OperationContext Renderer::start(renderer::IRenderTarget& target)
{
    renderer::OperationContext result;
    result.emplace<ogl::OperationContext>(this);

    if (!target.prepare(result))
    {
        result.emplace<vk::OperationContext>();
        return result;
    }

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_multisampling == Multisampling::MSA_1X)
    {
        return result;
    }
    else if (auto iter = m_samples.find(get(result).specificTarget->framebuffer());
             iter == m_samples.end())
    {
        SampleInfo& info = m_samples[get(result).specificTarget->framebuffer()];
        info.width = target.width();
        info.height = target.height();
        glEnable(GL_DEPTH_TEST);
        glGenFramebuffers(1, &info.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, info.framebuffer);

        glGenTextures(1, &info.texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, info.texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(m_multisampling),
            GL_RGB, target.width(), target.height(), GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
            info.texture, 0);

        glGenRenderbuffers(1, &info.renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, info.renderbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<GLsizei>(m_multisampling),
            GL_DEPTH24_STENCIL8, target.width(), target.height());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
            info.renderbuffer);
    }
    else if (iter->second.width != target.width() || iter->second.height != target.height())
    {
        iter->second.width = target.width();
        iter->second.height = target.height();

        glBindFramebuffer(GL_FRAMEBUFFER, iter->second.framebuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, iter->second.texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLsizei>(m_multisampling),
            GL_RGB, target.width(), target.height(), GL_TRUE);

        glBindRenderbuffer(GL_RENDERBUFFER, iter->second.renderbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, static_cast<GLsizei>(m_multisampling),
            GL_DEPTH24_STENCIL8, target.width(), target.height());
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, iter->second.framebuffer);
    }

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return result;
}

void Renderer::finish(renderer::OperationContext& context)
{
    if (m_multisampling != Multisampling::MSA_1X)
    {
        auto& specContext = get(context);
        GLuint targetFramebuffer = specContext.specificTarget->framebuffer();
        auto& sample = m_samples[targetFramebuffer];

        glBindFramebuffer(GL_READ_FRAMEBUFFER, sample.framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFramebuffer);
        glBlitFramebuffer(0, 0, sample.width, sample.height, 0, 0, sample.width, sample.height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    context.operationTarget().present(context);
}

}    //  namespace renderer::ogl
