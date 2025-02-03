#pragma once

#include <igraphics_pipeline.hpp>
#include <utils.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

class GraphicsContext;

class Pipeline : virtual public IPipeline
{
public:
    template <typename CreateInfoT>
    Pipeline(const GraphicsContext& context, CreateInfoT createInfo)
        : m_context(context)
    {
        init(createInfo.interfaceContainers(), createInfo.shaders());
    }

    virtual ~Pipeline();

protected:
    const GraphicsContext& m_context;

    GLuint m_shaderProgram;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_bindingIndices;
};

}    //  namespace renderer::ogl
